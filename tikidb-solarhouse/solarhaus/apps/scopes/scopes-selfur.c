#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"

#include "scopes-types.h"
#include "scopes.h"
#include "scopes-routing.h"
#include "scopes-selfur.h"

#ifdef FRAGMENTATION_ENABLED
#include "frag_unicast.h"
#endif

/* counts the number of scopes the local node has created */
static int scope_counter;

/* lock timer */
static struct etimer lock_timer;

/* hop count of node in tree */
static uint8_t hop_count = 0;
static uint8_t max_hop_count = 0;

/* prototypes */
static int receive_tree_update(struct netflood_conn *c, const rimeaddr_t *from,
		const rimeaddr_t *originator, uint8_t seqno, uint8_t hops);
static void receive_activation(struct runicast_conn *c, const rimeaddr_t *from,
		uint8_t seqno);
static void receive_hopcount(struct runicast_conn *c, const rimeaddr_t *from,
		uint8_t seqno);
static int receive_flood_data(struct netflood_conn *c, const rimeaddr_t *from,
		const rimeaddr_t *originator, uint8_t seqno, uint8_t hops);

#ifdef FRAGMENTATION_ENABLED
static void
receive_frag_unicast_data(struct frag_unicast_conn *c, const rimeaddr_t *from);
#else
static void
receive_unicast_data(struct unicast_conn *c, const rimeaddr_t *from);
#endif
static void receive_suppress(struct broadcast_conn *c, const rimeaddr_t *from);
static void build_routing_tree(void);
static void send_activation_msg(struct scope_entry *s);
static void send_hopcount_msg(void *r1);
static void add_routing_entry(const rimeaddr_t *root,
		const rimeaddr_t *next_hop);
static void update_routing_entry(struct routing_entry *r,
		const rimeaddr_t *next_hop);
static struct routing_entry *lookup_routing_entry(const rimeaddr_t *root);
static void print_routing_entry(struct routing_entry *r, char *msg);
static void remove_routing_entry(struct routing_entry *r);
static void add_scope_entry(scope_id_t scope_id, struct routing_entry *r);
static struct scope_entry *lookup_scope_entry(scope_id_t scope_id);
static void print_scope_entry(struct scope_entry *s, char *msg);

/* tree update connection and callbacks */
static struct netflood_conn tree_update;
static struct netflood_callbacks tree_update_callbacks = { receive_tree_update,
		NULL, NULL };
static uint8_t tree_seqno = 0;

/* data connection and callbacks for data to the scope members */
static struct netflood_conn data_flood;
static struct netflood_callbacks data_flood_callbacks = { receive_flood_data,
		NULL, NULL };
static uint8_t data_seqno = 0;

/* reliable unicast connection and callbacks for activation messages */
static struct runicast_conn activation_runicast;
static struct runicast_callbacks activation_callbacks = { receive_activation,
		NULL, NULL };

/* reliable unicast connection and callbacks for hopcount messages */
static struct runicast_conn hopcount_runicast;
static struct runicast_callbacks hopcount_callbacks = { receive_hopcount, NULL,
		NULL };

/* unicast connection and callbacks for data to the sink */
#ifdef FRAGMENTATION_ENABLED
static struct frag_unicast_conn data_unicast;
static struct frag_unicast_callbacks
data_unicast_callbacks = {receive_frag_unicast_data};
#else
static struct unicast_conn data_unicast;
static struct unicast_callbacks
		data_unicast_callbacks = { receive_unicast_data };
#endif

/* broadcast connection and callbacks for suppress messages */
static struct broadcast_conn suppress_broadcast;
static struct broadcast_callbacks suppress_broadcast_callbacks = {
		receive_suppress };

/* process declarations */
PROCESS(tree_update_process, "tree update process")
;
PROCESS(activation_process, "activation process")
;
PROCESS(info_process, "info process")
;

/* memory management */
MEMB(routing_mem, struct routing_entry, SELFUR_MAX_ROUTING_ENTRIES)
;
MEMB(scopes_mem, struct scope_entry, SELFUR_MAX_SCOPE_ENTRIES)
;
LIST(routing_list)
;
LIST(scopes_list)
;

/* XXX: added for TIKIDB support */
static struct ctimer tmp_timer;

struct routing_entry *
get_routing_list() {
	return list_head(routing_list);
}

struct scope_entry *
get_scopes_list() {
	return list_head(scopes_list);
}

uint8_t get_node_tree_height() {
	return hop_count;
}

uint8_t get_max_tree_height() {
	return max_hop_count;
}

uint8_t is_forwarder(uint8_t scope_id) {
	struct scope_entry * s = lookup_scope_entry(scope_id);

	if (SELFUR_HAS_STATUS(s, SELFUR_STATUS_FORWARDER)) {
		return 1;
	}

	return 0;
}

/* functions called by rime */
static int receive_tree_update(struct netflood_conn *c, const rimeaddr_t *from,
		const rimeaddr_t *originator, uint8_t seqno, uint8_t hops) {
	/* check if the local node is the originator of the message */
	if (!rimeaddr_cmp(&rimeaddr_node_addr, originator)) {
		/* look up the originator's routing entry */
		struct routing_entry *r = lookup_routing_entry(originator);
		if (r != NULL) {
			/* update the originator's existing routing entry */
			update_routing_entry(r, from);
		} else {
			/* add a new routing entry for the originator */
			add_routing_entry(originator, from);
		}

		/* XXX: store hop count, reset max hop count */
		hop_count = hops + 1;
		//		if (max_hop_count == 0 || max_hop_count > 10)
		max_hop_count = hops + 1;

		ctimer_set(&(tmp_timer), CLOCK_SECOND / 2 + random_rand()
				% CLOCK_SECOND, send_hopcount_msg, lookup_routing_entry(
				originator));

		/* rebroadcast the tree update */
		return 1;
	}

	/* do not rebroadcast the tree update if the local node is the originator */
	return 0;
}

static void receive_activation(struct runicast_conn *c, const rimeaddr_t *from,
		uint8_t seqno) {
	//	int hops;

	/* cast the message */
	struct route_activation_msg *msg =
			(struct route_activation_msg *) packetbuf_dataptr();

	//	hops = msg->max_hops;
	/* look up the scope's entry */
	struct scope_entry *s = lookup_scope_entry(msg->scope_id);
	if (s != NULL) {
		/* check if the activation should be forwarded to the next hop */
		if (/*!rimeaddr_cmp(&(s->tree->next_hop), &(s->tree->root)) && // XXX: for hop count */
		!(SELFUR_HAS_STATUS(s, SELFUR_STATUS_MEMBER)
				|| SELFUR_HAS_STATUS(s, SELFUR_STATUS_FORWARDER))) {
			SELFUR_SET_STATUS(s, SELFUR_STATUS_ACTIVATE);
		}

		if (!SELFUR_HAS_STATUS(s, SELFUR_STATUS_FORWARDER)) {
			/* set status forwarder */
			SELFUR_SET_STATUS(s, SELFUR_STATUS_FORWARDER);

			/* send suppression message */
			packetbuf_clear();
			struct route_suppress_msg *smsg =
					(struct route_suppress_msg *) packetbuf_dataptr();
			smsg->scope_id = s->scope_id;
			packetbuf_set_datalen(sizeof(struct route_suppress_msg));
			broadcast_send(&suppress_broadcast);
		}
	}

	//	// XXX: case for only hop-count
	//	if (hops > max_hop_count) {
	//		if (hops < 10)
	//			max_hop_count = hops;
	//		LOG("(ROUTING) max.hop-count: %d, hop-count: %d.\n", max_hop_count, hop_count);
	//		//			send_hopcount_msg(list_head(routing_list)); // TODO is this routing entry working every time ?!
	//		ctimer_set(&(tmp_timer), CLOCK_SECOND / 2, send_hopcount_msg,
	//				list_head(routing_list));
	//	}

}

static void receive_hopcount(struct runicast_conn *c, const rimeaddr_t *from,
		uint8_t seqno) {

	/* cast the message */
	struct hopcount_msg *msg = (struct hopcount_msg *) packetbuf_dataptr();

	// XXX: case for only hop-count
	if (msg->max_hops > max_hop_count) {
		//		if (hops < 10)
		max_hop_count = msg->max_hops;
		//			send_hopcount_msg(list_head(routing_list)); // TODO is this routing entry working every time ?!

		if (hop_count > 0) // If this node is not root, then send updated hop count to node's parent
			ctimer_set(&(tmp_timer), CLOCK_SECOND / 2, send_hopcount_msg,
					list_head(routing_list));
	}

}

static int receive_flood_data(struct netflood_conn *c, const rimeaddr_t *from,
		const rimeaddr_t *originator, uint8_t seqno, uint8_t hops) {
	/* whether to rebroadcast the message */
	int rebroadcast = 0;

	/* look up the root's routing entry */
	struct routing_entry *r = lookup_routing_entry(originator);
	if (r != NULL) {
		/* cast the message to the generic message type */
		struct scopes_msg_generic *gmsg =
				(struct scopes_msg_generic *) packetbuf_dataptr();
		struct scope_entry *s;

		/* check if the message should be rebroadcast */
		if (gmsg->scope_id == SCOPES_WORLD_SCOPE_ID || ((s
				= lookup_scope_entry(gmsg->scope_id)) != NULL
				&& SELFUR_HAS_STATUS(s, SELFUR_STATUS_FORWARDER))) {
			rebroadcast = 1;
		}

		/* check the message type */
		switch (gmsg->type) {
		case SCOPES_MSG_OPEN: {
			/* cast the message to the correct type */
			struct scopes_msg_open *msg =
					(struct scopes_msg_open *) packetbuf_dataptr();

			/* look up the sub scope's entry */
			s = lookup_scope_entry(msg->sub_scope_id);
			if (s == NULL) {
				/* add an entry for the sub scope */
				add_scope_entry(msg->sub_scope_id, r);
			}
			break;
		}
		}

		/* call scopes */
		scopes_receive(gmsg);
	}

	/* return rebroadcast decision */
	return rebroadcast;
}

#ifdef FRAGMENTATION_ENABLED
static void receive_frag_unicast_data(struct frag_unicast_conn *c, const rimeaddr_t *from)
#else
static void receive_unicast_data(struct unicast_conn *c, const rimeaddr_t *from)
#endif
{
	printf("(ROUTING) Scopes data received from %u.%u.\n", from->u8[0],
			from->u8[1]);

	/* cast message to the correct type */
	//  struct scopes_msg_data *msg = (struct scopes_msg_data *) packetbuf_dataptr();

	/* look up the scope's entry */
	//  struct scope_entry *s = lookup_scope_entry(msg->scope_id);
	//  if (s != NULL) {
	//
	//    /* check how to handle the message */
	//    if (rimeaddr_cmp(&rimeaddr_node_addr, &(s->tree->root))) {
	//      /* call scopes */
	//      scopes_receive();
	//    }
	//    else {
	//      /* forward to next hop // XXX: changed for AGGREGATION support */
#ifdef FRAGMENTATION_ENABLED
	scopes_receive(frag_unicast_buffer_ptr(&data_unicast));
#else
	scopes_receive(packetbuf_dataptr());
#endif
	//	if (s != NULL) {
	//      unicast_send(&data_unicast, &(s->tree->next_hop));
	//    }
	//   }
}

static void receive_suppress(struct broadcast_conn *c, const rimeaddr_t *from) {
	/* cast message to the correct type */
	struct route_suppress_msg *msg =
			(struct route_suppress_msg *) packetbuf_dataptr();

	/* look up the scope's entry */
	struct scope_entry *s = lookup_scope_entry(msg->scope_id);
	if (s != NULL) {
		/* check if the suppression message was sent by the local node's next hop */
		if (rimeaddr_cmp(&(s->tree->next_hop), from)) {
			/* set suppression status */
			SELFUR_SET_STATUS(s, SELFUR_STATUS_SUPPRESS);
		}
	}
}

/* interface functions */
static void init(void) {
	/* initialize memory */
	memb_init(&routing_mem);
	memb_init(&scopes_mem);
	list_init(routing_list);
	list_init(scopes_list);

	/* open a channel for tree updates */
	netflood_open(&tree_update, SELFUR_FLOODING_QUEUE_TIME,
			SELFUR_TREE_UPDATE_CHANNEL, &tree_update_callbacks);

	/* open a channel for activation messages */
	runicast_open(&activation_runicast, SELFUR_ACTIVATION_CHANNEL,
			&activation_callbacks);

	/* open a channel for activation messages */
	runicast_open(&hopcount_runicast, SELFUR_HOPCOUNT_CHANNEL,
			&hopcount_callbacks);

	/* open a channel for suppression messages */
	broadcast_open(&suppress_broadcast, SELFUR_SUPPRESS_CHANNEL,
			&suppress_broadcast_callbacks);

	/* open channels for data */
	netflood_open(&data_flood, SELFUR_FLOODING_QUEUE_TIME,
			SELFUR_DATA_FLOOD_CHANNEL, &data_flood_callbacks);

#ifdef FRAGMENTATION_ENABLED
	frag_unicast_open(&data_unicast, SELFUR_DATA_UNICAST_CHANNEL,&data_unicast_callbacks);
#else
	unicast_open(&data_unicast, SELFUR_DATA_UNICAST_CHANNEL,
			&data_unicast_callbacks);
#endif

	/* start tree update process */
	process_start(&tree_update_process, NULL);

	/* start activation process */
	process_start(&activation_process, NULL);

	/* start info process */
	process_start(&info_process, NULL);
}

static void send(struct scopes_msg_generic *gmsg) {
	/* check if the lock timer has expired */
	if (etimer_expired(&lock_timer)) {
		/* cast the message to the generic message type */

		//		struct scopes_msg_generic *gmsg;
//#ifdef FRAGMENTATION_ENABLED
//		gmsg = (struct scopes_msg_generic *) frag_unicast_buffer_ptr(&data_unicast);
//#else
//		gmsg = (struct scopes_msg_generic *) packetbuf_dataptr();
//#endif

		/* check the message type */
		if (gmsg->type == SCOPES_MSG_DATA) {
			/* cast the message to the correct type */
			struct scopes_msg_data *msg = (struct scopes_msg_data *) gmsg;

			/* check the direction of the message */
			if (msg->to_creator) {
				/* look up the scope's entry */
				struct scope_entry *s = lookup_scope_entry(msg->scope_id);
				if (s != NULL) {
#ifdef FRAGMENTATION_ENABLED
					frag_unicast_send(&data_unicast, &(s->tree->next_hop));
#else
					unicast_send(&data_unicast, &(s->tree->next_hop));
#endif
					return;
				}
			}
		}

		/* flood everything else */
		netflood_send(&data_flood, data_seqno++);
	}
}

static void add(struct scope *scope) {

	/* check if the local node is the root */
	if (HAS_STATUS(scope, SCOPES_STATUS_CREATOR)) {
		/* check scope counter */
		if (scope_counter == 0) {
			/* build routing tree */
			build_routing_tree();
		}

		/* increase scope counter */
		scope_counter++;
	}
}

static void buffer_clear() {
#ifdef FRAGMENTATION_ENABLED
	frag_unicast_buffer_clear(&data_unicast);
#else
	int i = 1;
	i++;
	printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! %u\n", i);
#endif
}

static uint8_t *buffer_ptr() {
#ifdef FRAGMENTATION_ENABLED
	return frag_unicast_buffer_ptr(&data_unicast);
#else
	return NULL;
#endif
}

static void buffer_setlen(uint16_t len) {
#ifdef FRAGMENTATION_ENABLED
	frag_unicast_buffer_setlen(&data_unicast, len);
#endif
}

//static uint16_t buffer_getlen() {
//#ifndef FRAGMENTATION_ENABLED
//	return 0;
//#else
//	return frag_unicast_buffer_getlen(&data_unicast);
//#endif
//}

static void remove(struct scope *scope) {

	/* check if the local node is the root */
	if (HAS_STATUS(scope, SCOPES_STATUS_CREATOR)) {
		/* check scope counter */
		if (scope_counter > 0) {
			/* decrease scope counter */
			scope_counter--;
		}
	}
}

static void join(struct scope *scope) {

	/* look up the scope's entry */
	struct scope_entry *s = lookup_scope_entry(scope->scope_id);
	if (s != NULL) {
		SELFUR_SET_STATUS(s, SELFUR_STATUS_MEMBER);
		/* check if the next hop is the root */
		//    if (!rimeaddr_cmp(&(s->tree->next_hop), &(s->tree->root))) {
		SELFUR_SET_STATUS(s, SELFUR_STATUS_ACTIVATE);
		//    } // XXX: commented for hop count
	}
}

static void leave(struct scope *scope) {

	/* look up the scope's entry */
	struct scope_entry *s = lookup_scope_entry(scope->scope_id);
	if (s != NULL) {
		SELFUR_UNSET_STATUS(s, SELFUR_STATUS_MEMBER);
	}
}
ROUTING(scopes_selfur,
		"selfur",
		init,
		send,
		add,
		remove,
		join,
		leave,
		buffer_clear,
		buffer_ptr,
		buffer_setlen);

/* helper functions */
static void build_routing_tree(void) {

	/* clear the contents of the packet buffer */
	packetbuf_clear();
	max_hop_count = 0; // XXX: hop count

	/* send a message with no payload */
	netflood_send(&tree_update, tree_seqno++);

	/* start the lock timer */
	etimer_set(&lock_timer, SELFUR_LOCK_TIMER_DURATION * CLOCK_SECOND);
}

static void send_activation_msg(struct scope_entry *s) {

	/* clear the contents of the packet buffer */
	packetbuf_clear();

	/* prepare the message */
	struct route_activation_msg *msg =
			(struct route_activation_msg *) packetbuf_dataptr();
	msg->scope_id = s->scope_id;
	//	msg->max_hops = max_hop_count; // XXX: for max hop count
	packetbuf_set_datalen(sizeof(struct route_activation_msg));

	/* send the message */
	runicast_send(&activation_runicast, &(s->tree->next_hop), SELFUR_ACTIVATION_RETRANSMISSIONS);
}

static void // XXX: for max hop count
//send_hopcount_msg(struct routing_entry *r)
send_hopcount_msg(void *r1) {

	if (r1 != NULL) {
		struct routing_entry *r = (struct routing_entry*) r1;

		/* clear the contents of the packet buffer */
		packetbuf_clear();

		/* prepare the message */
		struct hopcount_msg *msg = (struct hopcount_msg *) packetbuf_dataptr();
		msg->max_hops = max_hop_count;
		packetbuf_set_datalen(sizeof(struct hopcount_msg));

		/* send the message */
		if (!runicast_send(&hopcount_runicast, &(r->next_hop),
				SELFUR_ACTIVATION_RETRANSMISSIONS)) {
			ctimer_set(&(tmp_timer), CLOCK_SECOND / 2 + random_rand()
					% CLOCK_SECOND, send_hopcount_msg, list_head(routing_list));
		}
	}
}

static void add_routing_entry(const rimeaddr_t *root,
		const rimeaddr_t *next_hop) {
	/* try to get memory for the routing entry */
	struct routing_entry *r = (struct routing_entry *) memb_alloc(&routing_mem);
	if (r != NULL) {
		/* store the root's address */
		r->root = *root;

		/* store the next hop */
		update_routing_entry(r, next_hop);

		/* add the entry to the routing list */
		list_add(routing_list, r);
	}
}

static void update_routing_entry(struct routing_entry *r,
		const rimeaddr_t *next_hop) {
	/* set a new next hop */
	r->next_hop = *next_hop;

	/* reset the entry's timer */
	ctimer_set(&(r->ttl_timer), SELFUR_ROUTING_ENTRY_TTL
			* (clock_time_t) CLOCK_SECOND,
			(void(*)(void *)) remove_routing_entry, r);

	/* go through the scopes list */
	struct scope_entry *s;
	for (s = list_head(scopes_list); s != NULL; s = s->next) {
		/* check if the scope belongs to the tree that was updated */
		if (rimeaddr_cmp(&(s->tree->root), &(r->root))) {
			/* unset forwarder, suppress, and activate status indicators */
			SELFUR_UNSET_STATUS(s, SELFUR_STATUS_FORWARDER);
			SELFUR_UNSET_STATUS(s, SELFUR_STATUS_SUPPRESS);
			SELFUR_UNSET_STATUS(s, SELFUR_STATUS_ACTIVATE);

			/* check if an activation message needs to be sent to the next hop */
			if (SELFUR_HAS_STATUS(s, SELFUR_STATUS_MEMBER)) {
				SELFUR_SET_STATUS(s, SELFUR_STATUS_ACTIVATE);
			}
		}
	}
}

static struct routing_entry *
lookup_routing_entry(const rimeaddr_t *root) {
	/* go through the routing list */
	struct routing_entry *r;
	for (r = list_head(routing_list); r != NULL; r = r->next) {
		/* find the one with the matching root address */
		if (rimeaddr_cmp(root, &(r->root))) {
			return r;
		}
	}
	return NULL;
}

static void print_routing_entry(struct routing_entry *r, char *msg) {
}

static void remove_routing_entry(struct routing_entry *r) {
	/* go through the scopes list */
	struct scope_entry *current = list_head(scopes_list), *previous = NULL;
	while (current != NULL) {
		/* check if the scope belongs to the expired tree */
		if (rimeaddr_cmp(&(current->tree->root), &(r->root))) {
			/* remove the scope entry */
			if (previous == NULL) {
				list_remove(scopes_list, current);
				memb_free(&scopes_mem, current);
				current = list_head(scopes_list);
			} else {
				previous->next = current->next;
				memb_free(&scopes_mem, current);
				current = previous->next;
			}
		} else {
			previous = current;
			current = current->next;
		}
	}

	/* remove the routing entry and free its memory */
	list_remove(routing_list, r);
	memb_free(&routing_mem, r);
}

static void add_scope_entry(scope_id_t scope_id, struct routing_entry *r) {
	/* check if memory is available */
	struct scope_entry *s = (struct scope_entry *) memb_alloc(&scopes_mem);
	if (s != NULL) {
		/* store the scope id and associate the scope with a tree */
		s->scope_id = scope_id;
		s->status = SELFUR_STATUS_NONE;
		s->tree = r;

		/* add the entry to the scopes list */
		list_add(scopes_list, s);
	}
}

static struct scope_entry *
lookup_scope_entry(scope_id_t scope_id) {
	/* go through the scopes list */
	struct scope_entry *s;
	for (s = list_head(scopes_list); s != NULL; s = s->next) {
		/* find the one with the matching scope id */
		if (s->scope_id == scope_id) {
			return s;
		}
	}
	return NULL;
}


PROCESS_THREAD(tree_update_process, ev, data) {
	PROCESS_BEGIN();
	
		static struct etimer tree_update_timer;
		etimer_set(&tree_update_timer, SELFUR_ROUTING_REFRESH_INTERVAL * CLOCK_SECOND);

		while(1) {
			PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&tree_update_timer));
			etimer_reset(&tree_update_timer);

			/* check if the routing tree needs to be rebuilt */
			if (scope_counter > 0) {
				/* build routing tree */
				build_routing_tree();
			}
		}

		PROCESS_END();
	}


PROCESS_THREAD(activation_process, ev, data)
{
	PROCESS_BEGIN();

	static struct etimer activation_timer;
	etimer_set(&activation_timer, SELFUR_ACTIVATION_MAX_DELAY * CLOCK_SECOND + random_rand() % CLOCK_SECOND);
	
	while(1) {
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&activation_timer));
		etimer_set(&activation_timer, SELFUR_ACTIVATION_MAX_DELAY * CLOCK_SECOND + random_rand() % CLOCK_SECOND);

			/* send activations */
		struct scope_entry *s;
		for (s = list_head(scopes_list); s != NULL; s = s->next) {
			if (SELFUR_HAS_STATUS(s, SELFUR_STATUS_ACTIVATE) && !SELFUR_HAS_STATUS(s, SELFUR_STATUS_SUPPRESS)) {
				SELFUR_UNSET_STATUS(s, SELFUR_STATUS_ACTIVATE);
				send_activation_msg(s);
			}
		}
	}

	PROCESS_END();
}


PROCESS_THREAD(info_process, ev, data)
{
	PROCESS_BEGIN();
	
	static struct etimer info_timer;
	etimer_set(&info_timer, SELFUR_INFO_TIMER_DURATION * CLOCK_SECOND);

	while(1) {
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&info_timer));
		etimer_reset(&info_timer);
	}

	PROCESS_END();
}
