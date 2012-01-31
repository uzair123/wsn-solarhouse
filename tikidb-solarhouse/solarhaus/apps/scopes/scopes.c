#include <string.h>
#include <stdlib.h>

#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"
#include "ctimer.h"

#include "scopes-types.h"
#include "scopes-routing.h"
#include "scopes-membership.h"
#include "scopes-application.h"
#include "scopes.h"
#include "scopes-selfur.h"

/* preprocessor macros */
#define CALLBACK(subscriber, name_args)                \
  do {                                                 \
    PROCESS_CONTEXT_BEGIN((subscriber)->process);      \
    (subscriber)->application->name_args;              \
    PROCESS_CONTEXT_END((subscriber)->process);        \
  } while(0)

#define ARE_LINKED(super_scope, sub_scope)             \
  ((sub_scope)->super_scope_id == (super_scope)->scope_id)

/* static function declarations */
static struct scope *allocate_scope(data_len_t spec_len);
static void deallocate_scope(struct scope *scope);
static void add_scope(struct scope *scope);
static void remove_scope(struct scope *scope);
static void remove_scope_recursively(struct scope *scope);
static void remove_single_scope(struct scope *scope);
static void join_scope(struct scope *scope);
static void leave_scope(struct scope *scope);
static struct scope * lookup_scope_id(scope_id_t scope_id);
static void handle_scope_timeout(struct scope *scope);
static void reset_scope_timer(struct scope *scope);
static void announce_scope_open(struct scope *scope);
static void announce_scope_close(struct scope *scope);
static void print_scope(struct scope *scope);
static struct subscriber *lookup_subscriber_id(subscriber_id_t id);

/* declaration of scopes process */
PROCESS(scopes_process, "scopes process")
;

/* static memory for scopes information */
MEMB(scopes_mem, struct scope, SCOPES_MAX_SCOPES)
;

/* data structures for scope and subscriber management */
LIST(scopes)
;
LIST(subscribers)
;

/* scopes parameterization components */
static struct scopes_routing *routing;
static struct scopes_membership *membership;

/* the world scope */
struct scope world_scope;

static struct ctimer tmp_timer;
static struct scope mockup;

/* external function definitions */
void scopes_init(struct scopes_routing *r, struct scopes_membership *m) {

	/* initialize static memory for scopes structures */
	memb_init(&scopes_mem);

	/* initialize scopes list and subscriber list */
	list_init(scopes);
	list_init(subscribers);

	/* create world scope */
	world_scope.scope_id = SCOPES_WORLD_SCOPE_ID;
	world_scope.super_scope_id = SCOPES_WORLD_SCOPE_ID;
	world_scope.ttl = 0;
	world_scope.status = SCOPES_STATUS_MEMBER;
	world_scope.flags = SCOPES_FLAG_NONE;

	/* add world scope */
	list_add(scopes, &world_scope);

	/* set and initialize routing */
	routing = r;
	routing->init();

	/* set and initialize membership checker */
	membership = m;
	membership->init();

	/* start scopes process */
	process_start(&scopes_process, NULL);
}

int scopes_register(struct subscriber *subscriber, subscriber_id_t id,
		struct scopes_application *application) {
	/* first make sure that the subscriber hasn't subscribed before and
	 that the id is unused */
	struct subscriber *s;
	for (s = list_head(subscribers); s != NULL; s = s->next) {
		if (s == subscriber || s->id == id) {
			return 0;
		}
	}

	/* set id, status, application and process */
	subscriber->id = id;
	subscriber->status = SCOPES_APPLICATION_SUBSCRIBED;
	subscriber->application = application;
	subscriber->process = PROCESS_CURRENT();

	/* add the subscriber */
	list_add(subscribers, subscriber);
	return 1;
}

void scopes_unregister(struct subscriber *subscriber) {
	/* remove the subscriber */
	list_remove(subscribers, subscriber);
	subscriber->status = SCOPES_APPLICATION_UNSUBSCRIBED;
}

struct scope *
scopes_open(struct subscriber *subscriber, struct scope *super_scope,
		scope_id_t scope_id, void *specs, data_len_t spec_len,
		scope_flags_t flags, scope_ttl_t ttl) {


	/* check if the application is subscribed */
	if (!IS_SUBSCRIBED(subscriber)) {
		return NULL;
	}

	/* check if the scope id is already known */
	if (lookup_scope_id(scope_id) != NULL) {
		return NULL;
	}

	/* check if the super scope is known */
	if (lookup_scope_id(super_scope->scope_id) == NULL) {
		return NULL;
	}

	/* check if memory is available */
	struct scope *scope = allocate_scope(spec_len);
	if (scope == NULL) {
		return NULL;
	}

	/* fill in the scope information */
	scope->scope_id = scope_id;
	scope->super_scope_id = super_scope->scope_id;
	scope->owner = subscriber;
	scope->ttl = ttl;
	scope->status = SCOPES_STATUS_CREATOR;
	scope->flags = flags;
	scope->spec_len = spec_len;

	/* copy the specification */
	memcpy(scope->specs, specs, spec_len);

	/* add the scope to the scopes list */
	add_scope(scope);

	/* join the scope */
	join_scope(scope);

	/* announce the creation of the scope on the network */
	//  announce_scope_open(scope);
	ctimer_set(&(tmp_timer), CLOCK_SECOND,
			(void(*)(void *)) announce_scope_open, scope);

	/* return a reference to the new scope */
	return scope;
}

void scopes_close(struct subscriber *subscriber, struct scope *scope) {
	/* check if the application is subscribed */
	if (!IS_SUBSCRIBED(subscriber)) {
		return;
	}

	/* check if this scope may be deleted */
	if (!HAS_STATUS(scope, SCOPES_STATUS_CREATOR)
			|| !IS_OWNER(scope, subscriber)) {
		return;
	}

	/* announce the deletion of the scope on the network */
	announce_scope_close(scope);

	/* remove the scope from the scopes list */
	remove_scope(scope);
}

void scopes_send(struct subscriber *subscriber, struct scope *scope,
		bool to_creator, void *data, data_len_t data_len) {

#ifdef SCOPES_DEBUG
	printf("scopes::scopes_send() - function entered. subscriber id: %u, scope id: %u, , to_creator: %u, data_len: %u\n", subscriber->id, scope->scope_id, to_creator, data_len);
#endif //SCOPES_DEBUG

	/* check if the application is subscribed */
	if (!IS_SUBSCRIBED(subscriber)) {
		return;
	}

	/* check if a message may be sent to this scope */// XXX: added for aggregation support
	if (!HAS_STATUS(scope, SCOPES_STATUS_MEMBER) && !is_forwarder(
			scope->scope_id)) {
		return;
	}

	/* reset the contents of the packet buffer */
#ifdef FRAGMENTATION_ENABLED
	if (to_creator)
	routing->buffer_clear();
	else
	packetbuf_clear();
#else
	packetbuf_clear();
#endif

	/* create the message */
	struct scopes_msg_data *msg;
#ifdef FRAGMENTATION_ENABLED
	if (to_creator)
	msg = (struct scopes_msg_data *) routing->buffer_ptr();
	else
	msg = (struct scopes_msg_data *) packetbuf_dataptr();
#else
	msg = (struct scopes_msg_data *) packetbuf_dataptr();
#endif

	msg->scope_id = scope->scope_id;
	msg->type = SCOPES_MSG_DATA;
	msg->target_sid = subscriber->id;
	msg->to_creator = to_creator; // XXX: for TikiDB integration //HAS_STATUS(scope, SCOPES_STATUS_CREATOR) || IS_WORLD_SCOPE(scope) ? 0 : 1;
	msg->data_len = data_len;

	/* copy the payload in the packet buffer */
	void *data_ptr;

#ifdef FRAGMENTATION_ENABLED
	if (to_creator) {
		data_ptr = (void *) ((uint8_t *) routing->buffer_ptr()
				+ sizeof(struct scopes_msg_data));
		memcpy(data_ptr, data, data_len);
	}
	else {
		data_ptr = (void *) ((uint8_t *) packetbuf_dataptr()
				+ sizeof(struct scopes_msg_data));
		memcpy(data_ptr, data, data_len);
	}
#else
	data_ptr = (void *) ((uint8_t *) packetbuf_dataptr()
			+ sizeof(struct scopes_msg_data));
	memcpy(data_ptr, data, data_len);
#endif

	/* set the message length */
#ifdef FRAGMENTATION_ENABLED
	if (to_creator)
	routing->buffer_setlen(sizeof(struct scopes_msg_data) + data_len);
	else
		packetbuf_set_datalen(sizeof(struct scopes_msg_data) + data_len);
#else
	packetbuf_set_datalen(sizeof(struct scopes_msg_data) + data_len);
#endif

	/* tell the routing layer to send the packet */
	routing->send(msg);
#ifdef SCOPES_DEBUG
	LOG_L("Msg sent to routing.\n");
#endif //SCOPES_DEBUG
}

void scopes_receive(struct scopes_msg_generic *gmsg) {
#ifdef DEBUG_SCOPES
	printf("scope_id %d, type%d\n", gmsg->scope_id, gmsg->type);
#endif DEBUG_SCOPES
	switch (gmsg->type) {
	case SCOPES_MSG_OPEN: {
		/* cast the message to the correct type */
		struct scopes_msg_open *msg = (struct scopes_msg_open *) gmsg;

		struct scope *super_scope = lookup_scope_id(msg->scope_id);
		struct scope *new_scope = lookup_scope_id(msg->sub_scope_id);

		/* check if the super scope is known */
		if (super_scope != NULL) {
			/* check if the node is a member in the super scope */
			if (HAS_STATUS(super_scope, SCOPES_STATUS_MEMBER)) {
				/* check if the new scope is known */
				if (new_scope != NULL) {
					/* check if the new scope is the super scope's sub scope */
					if (ARE_LINKED(super_scope, new_scope)
							&& !HAS_STATUS(new_scope, SCOPES_STATUS_CREATOR)) {
						/* calculate the position of the specification */
						void *specs_msg_ptr = (void *) ((uint8_t *) gmsg
								+ sizeof(struct scopes_msg_open));

						/* check membership */
						int should_be_member = membership->check(specs_msg_ptr,
								msg->spec_len);

						/* check if the timer should be reset */// XXX: added isforwarder for aggregation
						if (should_be_member || msg->flags
								& SCOPES_FLAG_DYNAMIC || is_forwarder(
								msg->sub_scope_id)) {
							/* reset the sub scope's TTL timer */
							reset_scope_timer(new_scope);
						} else {
							/* remove the scope */
							remove_scope(new_scope);
						}
					}
				} else {
					/* check if the owner exists on this node and is subscribed */
					struct subscriber *owner = lookup_subscriber_id(
							msg->owner_sid);

					if (owner != NULL && IS_SUBSCRIBED(owner)) {
						/* calculate the position of the specification */
						void *specs_msg_ptr =
								(void *) ((uint8_t *) packetbuf_dataptr()
										+ sizeof(struct scopes_msg_open));

						/* check membership */
						int should_be_member = membership->check(specs_msg_ptr,
								msg->spec_len);

						/* add the scope if the node meets the membership criteria
						 or if the scope is a dynamic scope */// XXX: added isforwarder for aggregation
						if (should_be_member || msg->flags
								& SCOPES_FLAG_DYNAMIC || is_forwarder(
								msg->sub_scope_id)) {
							/* try to get memory for the new scope */
							new_scope = allocate_scope(msg->spec_len);

							/* check if memory could be obtained */
							if (new_scope != NULL) {
								/* fill in the scope information */
								new_scope->scope_id = msg->sub_scope_id;
								new_scope->super_scope_id = msg->scope_id;
								new_scope->owner = owner;
								new_scope->ttl = msg->ttl;
								new_scope->status = SCOPES_STATUS_NONE;
								new_scope->flags = msg->flags;
								new_scope->spec_len = msg->spec_len;

								/* copy the specification */
								memcpy(new_scope->specs, specs_msg_ptr,
										msg->spec_len);

								/* add the scope to the scopes list */
								add_scope(new_scope);

								/* join if the node should be a member */
								if (should_be_member) {
									join_scope(new_scope);
								}
							}
						}
					}
				}
			}
		}
	}
		break;
	case SCOPES_MSG_CLOSE: {
		/* cast the message to the correct type */
		struct scopes_msg_close *msg = (struct scopes_msg_close *) gmsg;

		struct scope *super_scope = lookup_scope_id(msg->scope_id);
		struct scope *sub_scope = lookup_scope_id(msg->sub_scope_id);

		/* check if the scope should be removed */
		if (super_scope != NULL && sub_scope != NULL
				&& ARE_LINKED(super_scope, sub_scope)
				&& !HAS_STATUS(sub_scope, SCOPES_STATUS_CREATOR)) {
			/* remove the scope */
			remove_scope(sub_scope);
		}
	}
		break;
	case SCOPES_MSG_DATA: {
		/* cast the message to the correct type */
		struct scopes_msg_data *msg = (struct scopes_msg_data *) gmsg;

		struct scope *scope = lookup_scope_id(msg->scope_id);

		/* check if the scope is known */
		if (scope != NULL) {
			/* check if the node is a member */
			//		  if (HAS_STATUS(scope, SCOPES_STATUS_MEMBER)) { // XXX: removed for aggregation support
			/* check if the message should be delivered */
			if ((msg->to_creator /*&& HAS_STATUS(scope, SCOPES_STATUS_CREATOR) // XXX: commented for AGGREGATION support*/)
					|| (!(msg->to_creator)
							&& !HAS_STATUS(scope, SCOPES_STATUS_CREATOR)
							&& HAS_STATUS(scope, SCOPES_STATUS_MEMBER) /*  // XXX: added for AGGREGATION support */)) {
				/* check if the owner is subscribed */


				if (IS_SUBSCRIBED(scope->owner)) {
					/* calculate the position of the payload */
					void *payload_ptr = (void *) ((uint8_t *) gmsg
							+ sizeof(struct scopes_msg_data));

					/* notify the owner */
					CALLBACK(scope->owner, recv(scope, payload_ptr, msg->data_len));
				}
			}
			//        }
		} else {
			// with no scope entry it is a forwarder! or not :( // XXX: added for aggregation support
			if (is_forwarder(msg->scope_id) && msg->to_creator) {
				struct subscriber *s = lookup_subscriber_id(msg->target_sid);

				if (IS_SUBSCRIBED(s)) {
					// calculate the position of the payload
					void *payload_ptr =
							(void *) ((uint8_t *) packetbuf_dataptr()
									+ sizeof(struct scopes_msg_data));

					// artificial scope entry
					mockup.scope_id = msg->scope_id;
					mockup.super_scope_id = 0;
					mockup.owner = s;

					// notify the owner
					CALLBACK(s, recv(&mockup, payload_ptr, msg->data_len));
				}
			}
		}

	}
		break;
	default:
		return;
	}
}

struct scope *
scopes_lookup(scope_id_t scope_id) {
	return lookup_scope_id(scope_id);
}

/* static function definitions */
static struct scope *
allocate_scope(data_len_t spec_len) {
	/* try to get memory for the scope */
	struct scope *scope = (struct scope *) memb_alloc(&scopes_mem);

	/* check if memory could be obtained */
	if (scope == NULL) {
		return NULL;
	}

	/* try to get memory for the specification */
	void *specs_ptr = malloc(spec_len);

	/* check if memory could be obtained */
	if (specs_ptr == NULL) {
		/* free memory for scope structure */
		memb_free(&scopes_mem, scope);
		return NULL;
	}

	/* set memory address of specification */
	scope->specs = specs_ptr;

	/* return new scope */
	return scope;
}

static void deallocate_scope(struct scope *scope) {
	/* free specification memory */
	free(scope->specs);

	/* free scope memory */
	memb_free(&scopes_mem, scope);
}

static void add_scope(struct scope *scope) {
	/* start the TTL timer for this scope */
	reset_scope_timer(scope);

	/* add the scope to the scopes list */
	list_add(scopes, scope);

	print_scope(scope);

	/* inform the routing processes */
	routing->add(scope);

	/* inform the owner */
	CALLBACK(scope->owner, add(scope));
}

static void remove_scope(struct scope *scope) {
	if (!IS_WORLD_SCOPE(scope)) {
		/* recursively delete all sub-scopes */
		remove_scope_recursively(scope);

		/* remove the scope */
		remove_single_scope(scope);
	}
}

static void remove_scope_recursively(struct scope *scope) {
	struct scope *current, *previous;
	current = list_head(scopes);
	previous = NULL;

	while (current != NULL) {
		if (current->super_scope_id == scope->scope_id) {
			remove_scope_recursively(current);
			remove_single_scope(current);

			if (previous == NULL) {
				current = list_head(scopes);
			} else {
				current = previous;
			}
		}
		previous = current;
		if (current != NULL) {
			current = current->next;
		}
	}
}

static void remove_single_scope(struct scope *scope) {
	/* leave scope in case node is a member */
	if (HAS_STATUS(scope, SCOPES_STATUS_MEMBER)) {
		leave_scope(scope);
	}

	/* inform the owner */
	CALLBACK(scope->owner, remove(scope));

	/* inform the routing process */
	routing->remove(scope);

	/* remove the scope */
	list_remove(scopes, scope);

	/* stop the scope's TTL timer */
	ctimer_stop(&(scope->ttl_timer));

	/* deallocate scope memory */
	deallocate_scope(scope);
}

static void join_scope(struct scope *scope) {
	/* set member status */
	scope->status |= SCOPES_STATUS_MEMBER;

	/* inform the routing processes */
	routing->join(scope);

	/* inform the owner */
	CALLBACK(scope->owner, join(scope));
}

static void leave_scope(struct scope *scope) {
	/* inform the owner */
	CALLBACK(scope->owner, leave(scope));

	/* inform the routing process */
	routing->leave(scope);

	/* unset member status */
	scope->status &= ~SCOPES_STATUS_MEMBER;
}

static struct scope *
lookup_scope_id(scope_id_t scope_id) {
	struct scope *s;

	/* go through the list of scopes and find the one with the given id */
	for (s = list_head(scopes); s != NULL; s = s->next) {
		if (s->scope_id == scope_id) {
			return s;
		}
	}
	return NULL;
}

static void handle_scope_timeout(struct scope *scope) {
	/* check if the node is the creator of the scope and if the owner is
	 subscribed */
	if (HAS_STATUS(scope, SCOPES_STATUS_CREATOR) && IS_SUBSCRIBED(scope->owner)) {
		/* re-announce the scope on the network */
		announce_scope_open(scope);

		/* reset the timer to ensure periodic re-announcements */
		reset_scope_timer(scope);
	} else {
		/* remove the expired scope */
		remove_scope(scope);
	}
}

static void reset_scope_timer(struct scope *scope) {
	clock_time_t timeout = CLOCK_SECOND * scope->ttl;

	if (HAS_STATUS(scope, SCOPES_STATUS_CREATOR)) {
		/* shorten the creator's timer so the scope is re-announced in time */
		timeout = timeout * SCOPES_REANNOUNCE_FACTOR;
	}

	/* reset the timer */
	ctimer_set(&(scope->ttl_timer), timeout,
			(void(*)(void *)) handle_scope_timeout, scope);
}

static void announce_scope_open(struct scope *scope) {
	/* reset the contents of the packet buffer */
	packetbuf_clear();

	/* create the message */
	struct scopes_msg_open *msg =
			(struct scopes_msg_open *) packetbuf_dataptr();
	msg->scope_id = scope->super_scope_id;
	msg->type = SCOPES_MSG_OPEN;
	msg->sub_scope_id = scope->scope_id;
	msg->owner_sid = scope->owner->id;
	msg->ttl = scope->ttl;
	msg->flags = scope->flags;
	msg->spec_len = scope->spec_len;

	/* copy the scope specifications in the packet buffer */
	void *specs_ptr = (void *) ((uint8_t *) packetbuf_dataptr()
			+ sizeof(struct scopes_msg_open));
	memcpy(specs_ptr, scope->specs, scope->spec_len);

	/* set the message length */
	packetbuf_set_datalen(sizeof(struct scopes_msg_open) + scope->spec_len);

	/* tell the routing layer to send the packet */
	routing->send(msg);
}

static void announce_scope_close(struct scope *scope) {
	/* reset the contents of the packet buffer */
	packetbuf_clear();

	/* create the message */
	struct scopes_msg_close *msg =
			(struct scopes_msg_close *) packetbuf_dataptr();
	msg->scope_id = scope->super_scope_id;
	msg->type = SCOPES_MSG_CLOSE;
	msg->sub_scope_id = scope->scope_id;

	/* set the message length */
	packetbuf_set_datalen(sizeof(struct scopes_msg_close));

	/* tell the routing layer to send the packet */
	routing->send(msg);
}

static void print_scope(struct scope *scope) {
	LOG_L("scope-id=%u, super-scope-id=%u, owner-sid=%u, ttl=%u, flags=%u, status=%u, spec-len=%u\n",
			scope->scope_id,
			scope->super_scope_id,
			scope->owner->id,
			timer_remaining(&(scope->ttl_timer.etimer.timer)) / CLOCK_SECOND,
			scope->flags,
			scope->status,
			scope->spec_len);
}

static struct subscriber *
lookup_subscriber_id(subscriber_id_t id) {
	struct subscriber *s;

	/* go through the list of subscribers and find the one with the given id */
	for (s = list_head(subscribers); s != NULL; s = s->next) {
		if (s->id == id) {
			return s;
		}
	}
	return NULL;
}

/* scopes process */
PROCESS_THREAD(scopes_process, ev, data) {
	PROCESS_BEGIN();

		LOG_L("scopes process started\n");

		/* create and start an event timer */
		static struct etimer scopes_timer;
		etimer_set(&scopes_timer, SCOPES_TIMER_DURATION * CLOCK_SECOND);

		do {
			/* wait till the timer expires and then reset it immediately */
			PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&scopes_timer));
			etimer_reset(&scopes_timer);

			struct scope *s;

			/* check memberships of dynamic scopes */
			//			LOG_L("checking dynamic scope memberships\n");
			for (s = list_head(scopes); s != NULL; s = s->next) {
				/* only check scopes not created by the local node */
				if (HAS_FLAG(s, SCOPES_FLAG_DYNAMIC) && !HAS_STATUS(s, SCOPES_STATUS_CREATOR)) {
					/* check membership */
					int should_be_member = membership->check(s->specs, s->spec_len);

					/* decide action */
					if (should_be_member && !HAS_STATUS(s, SCOPES_STATUS_MEMBER)) {
						/* join scope */
						join_scope(s);
					}
					else if (!should_be_member && HAS_STATUS(s, SCOPES_STATUS_MEMBER)) {
						/* leave scope */
						leave_scope(s);
					}
				}
			}

			//			/* print scopes information */
			//			LOG_L("known scopes:\n");
			//			LOG_L("-------------\n");
			//			for (s = list_head(scopes); s != NULL; s = s->next) {
			//				print_scope(s);
			//			}
			//			LOG_L("-------------\n");

		}while(1);

		PROCESS_END();
	}
