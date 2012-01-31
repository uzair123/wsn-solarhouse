#include <string.h>
#include <stdlib.h>

#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"
#include "sys/ctimer.h"

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
static struct scopes_subscriber *lookup_subscriber_id(subscriber_id_t id);

/* function declarations (functions used externally) */
void scopes_init(struct scopes_routing *routing,
		struct scopes_membership *membership);
int scopes_register(subscriber_id_t id, struct scopes_application *application);
void scopes_unregister(subscriber_id_t subscriber_id);
bool scopes_open(subscriber_id_t subscriber_id, scope_id_t super_scope,
		scope_id_t scope_id, void *specs, data_len_t spec_len,
		scope_flags_t flags, scope_ttl_t ttl);
void scopes_close(subscriber_id_t subscriber_id, scope_id_t scope_id);
void scopes_send(subscriber_id_t subscriber_id, scope_id_t scope_id,
		bool to_creator, void *data, data_len_t data_len);
void scopes_receive(struct scopes_msg_generic *gmsg);


/* declaration of scopes process */
PROCESS(scopes_process, "scopes process")
;

/** \brief Static memory for scopes' information */
MEMB(scopes_mem, struct scope, SCOPES_MAX_SCOPES)
;
/** \brief Static memory for scopes' subscribers */
MEMB(subscribers_mem, struct scopes_subscriber, SCOPES_MAX_SUBSCRIBER)
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

/** \brief Timer to delay scope announcement */
static struct ctimer anndelay_timer;

//static struct ctimer tmp_timer;
static struct scope mockup;

/* external function definitions */
/** \brief Initializer of the Scopes framework */
void scopes_init(struct scopes_routing *r, struct scopes_membership *m) {
	PRINTF(2,"(SCOPES) initializing scopes...\n");

	/* initialize static memory for scopes structures */
	memb_init(&scopes_mem);
	memb_init(&subscribers_mem);

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
	PRINTF(2,"(SCOPES) using routing: %s\n", r->name);
	routing = r;
	routing->init();

	/* set and initialize membership checker */
	PRINTF(2,"(SCOPES) using membership: %s\n", m->name);
	membership = m;
	membership->init();

	/* start scopes process */
	process_start(&scopes_process, NULL);
}

int scopes_register(subscriber_id_t id, struct scopes_application *application) {
	/* first make sure that the subscriber hasn't subscribed before and
	 that the id is unused */
	struct scopes_subscriber *s;
	for (s = list_head(subscribers); s != NULL; s = s->next) {
		if (s->id == id) {
			PRINTF(3,"(SCOPES) process is already subscribed to scopes\n");
			return 0;
		}
	}

	struct scopes_subscriber *subscriber =
			(struct scopes_subscriber *) memb_alloc(&subscribers_mem);

	if (subscriber == NULL) {
		PRINTF(3,"(SCOPES) Could not allocate memory for subscriber %d.\n", id);
		return 0;
	}

	/* set id, status, application and process */
	subscriber->id = id;
	subscriber->status = SCOPES_APPLICATION_SUBSCRIBED;
	subscriber->application = application;
	subscriber->process = PROCESS_CURRENT();

	/* add the subscriber */
	list_add(subscribers, subscriber);
	PRINTF(2,"(SCOPES) scopes_register() : added subscriber: %s (sid=%u)\n", subscriber->process->name, subscriber->id);
	return 1;
}

void scopes_unregister(subscriber_id_t subscriber_id) {
	/* check if the super scope is known */
	struct scopes_subscriber *subscriber = lookup_subscriber_id(subscriber_id);
	if (subscriber == NULL) {
		PRINTF(3,"(SCOPES) subscriber does not exist: %u\n", subscriber_id);
		return;
	}

	/* remove the subscriber */
	list_remove(subscribers, subscriber);
	PRINTF(3,"(SCOPES) removed subscriber: %s (sid=%u)\n", subscriber->process->name, subscriber->id);
	memb_free(&subscribers_mem, subscriber);
}

/** \brief @todo */
bool scopes_open(subscriber_id_t subscriber_id, scope_id_t super_scope_id,
		scope_id_t scope_id, void *specs, data_len_t spec_len,
		scope_flags_t flags, scope_ttl_t ttl) {
	struct scopes_subscriber *subscriber = lookup_subscriber_id(subscriber_id);
	struct scope *super_scope = NULL;
	struct scope *scope;

	/* check if the application is subscribed */
	if (subscriber != NULL && !IS_SUBSCRIBED(subscriber)) {
		PRINTF(3,"(SCOPES) application not subscribed or not found\n");
		return FALSE;
	}

	/* check if the scope id is already known */
	if ((scope = lookup_scope_id(scope_id)) != NULL) {
//		printf("increasing, %u\n",scope->use_counter);
		scope->use_counter++;
		//		PRINTF(3,"(SCOPES) scope id already in use: %u\n", scope_id);
		//		return FALSE;
		return TRUE;
	}

	/* check if the super scope is known */
	super_scope = lookup_scope_id(super_scope_id);
	if (super_scope == NULL) {
		PRINTF(3,"(SCOPES) super scope does not exist: %u\n", super_scope_id);
		return FALSE;
	}

	/* check if memory is available */
	scope = allocate_scope(spec_len);
	if (scope == NULL) {
		PRINTF(3,"(SCOPES) can't get memory for scope: %u\n", scope_id);
		return FALSE;
	}

	/* fill in the scope information */
	scope->scope_id = scope_id;
	scope->super_scope_id = super_scope->scope_id;
	scope->owner = subscriber;
	scope->use_counter = 1;
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

	/* set timer for the announcement of the creation of the scope on the network */
	ctimer_set(&(anndelay_timer), CLOCK_SECOND,
			(void(*)(void *)) announce_scope_open, scope);

	/* return TRUE since scope was created correctly */
	return TRUE;
}

/** \brief Returns whether this node is member of the scope with id passed as parameter */
bool scopes_member_of(scope_id_t scope_id) {
	struct scope *scope = lookup_scope_id(scope_id);
	return HAS_STATUS(scope, SCOPES_STATUS_MEMBER);
}

/** \brief Returns whether this node is creator (root) of the scope with id passed as parameter */
bool scopes_creator_of(scope_id_t scope_id) {
	struct scope *scope = lookup_scope_id(scope_id);
	return HAS_STATUS(scope, SCOPES_STATUS_CREATOR);
}


void scopes_close(subscriber_id_t subscriber_id, scope_id_t scope_id) {
	struct scopes_subscriber *subscriber = lookup_subscriber_id(subscriber_id);
	struct scope *scope = NULL;

	/* check if the application is subscribed */
	if (subscriber != NULL && !IS_SUBSCRIBED(subscriber)) {
		PRINTF(3,"(SCOPES) application not subscribed or not found\n");
		return;
	}

	/* check if the scope is known */
	scope = lookup_scope_id(scope_id);
	if (scope == NULL) {
		PRINTF(3,"(SCOPES) scope does not exist: %u\n", scope_id);
		return;
	}

	/* check if this scope may be deleted */
	if (!HAS_STATUS(scope, SCOPES_STATUS_CREATOR)
			|| !IS_OWNER(scope, subscriber)) {
		PRINTF(3,"(SCOPES) not allowed to delete scope: %u\n", scope->scope_id);
		return;
	}

//	printf("count: %u\n", scope->use_counter);
//	printf("ttl: %u\n", scope->ttl);
	if (--scope->use_counter == 0) {

		/* announce the deletion of the scope on the network */
		announce_scope_close(scope);

		/* remove the scope from the scopes list */
		remove_scope(scope);
	}

}

void scopes_send(subscriber_id_t subscriber_id, scope_id_t scope_id,
		bool to_creator, void *data, data_len_t data_len) {
	struct scopes_subscriber *subscriber = lookup_subscriber_id(subscriber_id);
	struct scope *scope = NULL;

//	int x;
//	for (x=0; x < data_len;x++)printf("%u,",((uint8_t*)data)[x]);
//	printf("\n");
	/* check if the application is subscribed */
	if (subscriber != NULL && !IS_SUBSCRIBED(subscriber)) {
		PRINTF(3,"(SCOPES) application not subscribed or found\n");
		return;
	}

	/* check if the scope is known */
	scope = lookup_scope_id(scope_id);
	if (scope == NULL) {
		PRINTF(3,"(SCOPES) scope does not exist: %u\n", scope_id);
		return;
	}


	/* check if a message may be sent to this scope */
	if (!HAS_STATUS(scope, SCOPES_STATUS_MEMBER) //
			&& !scopes_selfur_is_forwarder(scope_id)) { // this line added for aggregation support
		PRINTF(3,"(SCOPES) not allowed to send to scope: %u\n", scope->scope_id);
		return;
	}

	/* reset the contents of the message buffer */
	routing->buffer_clear(to_creator);

	/* create the message */
	struct scopes_msg_data *msg =
			(struct scopes_msg_data *) routing->buffer_ptr(to_creator);

	msg->scope_id = scope->scope_id;
	msg->type = SCOPES_MSG_DATA;
	msg->target_sid = subscriber->id;
	msg->to_creator = to_creator;
	msg->data_len = data_len;
	rimeaddr_copy(&msg->source, &rimeaddr_node_addr);

	/* copy the payload in the packet buffer */
	void *data_ptr =
			(void *) ((uint8_t *) msg + sizeof(struct scopes_msg_data));
	memcpy(data_ptr, data, data_len);

	/* set the message length */
	routing->buffer_setlen(to_creator, sizeof(struct scopes_msg_data)
			+ data_len);

	/* tell the routing layer to send the packet */
	routing->send(scope->scope_id, to_creator);
}

/**
 * \brief TODO
 */
void scopes_receive(struct scopes_msg_generic *gmsg) {


	PRINTF(2,"(SCOPES) scopes_receive() : scopes_receive: type %u\n", gmsg->type);

	/* now we can access the type field in the message */
	switch (gmsg->type) {
	case SCOPES_MSG_OPEN: {
		/* cast the message to the correct type */
		struct scopes_msg_open *msg = (struct scopes_msg_open *) gmsg;

		PRINTF(2,"(SCOPES) SCOPES_MSG_OPEN: scope-id=%u, subscope-id=%u, owner-sid=%u, ttl=%u, flags=%u, spec-len=%u\n",
				msg->scope_id, msg->sub_scope_id, msg->owner_sid, msg->ttl, msg->flags, msg->spec_len);

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
								& SCOPES_FLAG_DYNAMIC || scopes_selfur_is_forwarder(
								msg->sub_scope_id)) {
							PRINTF(3,"(SCOPES) received re-announcement for scope: %u\n", new_scope->scope_id);
							/* reset the sub scope's TTL timer */
							reset_scope_timer(new_scope);
						} else {
							/* remove the scope */
							remove_scope(new_scope);
						}
					}
				} else {
					/* check if the owner exists on this node and is subscribed */
					struct scopes_subscriber *owner = lookup_subscriber_id(
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
								& SCOPES_FLAG_DYNAMIC || scopes_selfur_is_forwarder(
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

		PRINTF(2,"(SCOPES) SCOPES_MSG_CLOSE: scope-id=%u, subscope-id=%u\n", msg->scope_id, msg->sub_scope_id);

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

		PRINTF(2,"(SCOPES) scopes_receive() : SCOPES_MSG_DATA: scope-id=%u, target_sid=%u, to-creator=%u, data-len=%u\n",
				msg->scope_id, msg->target_sid, msg->to_creator, msg->data_len);

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

				PRINTF(2,"(SCOPES) scopes_receive() : SCOPES_MSG_DATA scope owner app id %u, status is %u \n", scope->owner->id, scope->owner->status);

				if (IS_SUBSCRIBED(scope->owner)) {
					/* calculate the position of the payload */
					void *payload_ptr = (void *) ((uint8_t *) gmsg
							+ sizeof(struct scopes_msg_data));

					/* notify the owner */
					PRINTF(2,"(SCOPES) scopes_receive() : SCOPES_MSG_DATA: before the callback\n");
					CALLBACK(scope->owner, recv(scope->scope_id, payload_ptr, msg->data_len, msg->to_creator, &msg->source));
				}
			}
			//        }
		} else {
			// with no scope entry it is a forwarder! or not :( // XXX: added for aggregation support
			if (scopes_selfur_is_forwarder(msg->scope_id) && msg->to_creator) {
				struct scopes_subscriber *s = lookup_subscriber_id(msg->target_sid);

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
					CALLBACK(s, recv(msg->scope_id, payload_ptr, msg->data_len, msg->to_creator, &msg->source));
				}
			}
		}

	}
		break;
	default:
		PRINTF(2,"(SCOPES) unknown message type received: %u\n", gmsg->type);
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

	PRINTF(2,"(SCOPES) scope added: ");
	print_scope(scope);

	/* inform the routing processes */
	routing->add(scope->scope_id, HAS_STATUS(scope, SCOPES_STATUS_CREATOR));

	/* inform the owner */
	CALLBACK(scope->owner, add(scope->scope_id));
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
	CALLBACK(scope->owner, remove(scope->scope_id));

	/* inform the routing process */
	routing->remove(scope->scope_id);

	/* remove the scope */
	list_remove(scopes, scope);
	PRINTF(2,"(SCOPES) removed scope: %u\n", scope->scope_id);

	/* stop the scope's TTL timer */
	ctimer_stop(&(scope->ttl_timer));

	/* deallocate scope memory */
	deallocate_scope(scope);
}

static void join_scope(struct scope *scope) {
	/* set member status */
	scope->status |= SCOPES_STATUS_MEMBER;

	PRINTF(2,"(SCOPES) joined scope: %u\n", scope->scope_id);

	/* inform the routing processes */
	routing->join(scope->scope_id);

	/* inform the owner */
	CALLBACK(scope->owner, join(scope->scope_id));
}

static void leave_scope(struct scope *scope) {
	PRINTF(2,"(SCOPES) left scope: %u\n", scope->scope_id);

	/* inform the owner */
	CALLBACK(scope->owner, leave(scope->scope_id));

	/* inform the routing process */
	routing->leave(scope->scope_id);

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
		PRINTF(3,"(SCOPES) re-announcing scope: %u\n", scope->scope_id);
		announce_scope_open(scope);

		/* reset the timer to ensure periodic re-announcements */
		reset_scope_timer(scope);
	} else {
		/* remove the expired scope */
		PRINTF(3,"(SCOPES) scope has expired: %u\n", scope->scope_id);
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
	PRINTF(3,"(SCOPES) resetting ttl timer of scope %u to %lu, remained %lu\n", scope->scope_id, timeout, timer_remaining(&(scope->ttl_timer.etimer.timer)));
	ctimer_set(&(scope->ttl_timer), timeout,
			(void(*)(void *)) handle_scope_timeout, scope);
}

static void announce_scope_open(struct scope *scope) {
	PRINTF(2,"(SCOPES) announcing scope %u\n", scope->scope_id);
	/* reset the contents of the packet buffer */
	routing->buffer_clear(FALSE);

	/* create the message */
	struct scopes_msg_open *msg =
			(struct scopes_msg_open *) routing->buffer_ptr(FALSE);

	msg->scope_id = scope->super_scope_id;
	msg->type = SCOPES_MSG_OPEN;
	msg->sub_scope_id = scope->scope_id;
	msg->owner_sid = scope->owner->id;
	msg->ttl = scope->ttl;
	msg->flags = scope->flags;
	msg->spec_len = scope->spec_len;

	/* copy the scope specifications in the packet buffer */
	void *specs_ptr = (void *) ((uint8_t *) routing->buffer_ptr(FALSE)
			+ sizeof(struct scopes_msg_open));
	memcpy(specs_ptr, scope->specs, scope->spec_len);

	/* set the message length */
	routing->buffer_setlen(FALSE, sizeof(struct scopes_msg_open)
			+ scope->spec_len);

	/* tell the routing layer to send the packet */
	routing->send(scope->super_scope_id, FALSE);
}

static void announce_scope_close(struct scope *scope) {
	/* reset the contents of the packet buffer */
	routing->buffer_clear(FALSE);

	/* create the message */
	struct scopes_msg_close *msg =
			(struct scopes_msg_close *) routing->buffer_ptr(FALSE);
	msg->scope_id = scope->super_scope_id;
	msg->type = SCOPES_MSG_CLOSE;
	msg->sub_scope_id = scope->scope_id;

	/* set the message length */
	routing->buffer_setlen(FALSE, sizeof(struct scopes_msg_close));

	/* tell the routing layer to send the packet */
	routing->send(scope->super_scope_id, FALSE);
}

static void print_scope(struct scope *scope) {
	PRINTF(2,"(SCOPES) scope-id=%u, super-scope-id=%u, owner-sid=%u, ttl=%lu, flags=%u, status=%u, spec-len=%u\n",
			scope->scope_id,
			scope->super_scope_id,
			scope->owner->id,
			timer_remaining(&(scope->ttl_timer.etimer.timer)) / CLOCK_SECOND,
			scope->flags,
			scope->status,
			scope->spec_len);
}

static struct scopes_subscriber *
lookup_subscriber_id(subscriber_id_t id) {
	struct scopes_subscriber *s;

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

		PRINTF(2,"(SCOPES) scopes process started\n");

		/* create and start an event timer */
		static struct etimer scopes_timer;
		etimer_set(&scopes_timer, SCOPES_TIMER_DURATION * CLOCK_SECOND);

		do {
			/* wait till the timer expires and then reset it immediately */
			PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&scopes_timer));
			etimer_reset(&scopes_timer);

			struct scope *s;

			/* check memberships of dynamic scopes */
			//			PRINTF(2,"(SCOPES) checking dynamic scope memberships\n");
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
			//			PRINTF(2,"known scopes:\n");
			//			PRINTF(2,"-------------\n");
			//			for (s = list_head(scopes); s != NULL; s = s->next) {
			//				print_scope(s);
			//			}
			//			PRINTF(2,"-------------\n");

		}while(1);

		PROCESS_END();
	}
