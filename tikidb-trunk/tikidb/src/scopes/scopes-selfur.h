#ifndef __SCOPES_SELFUR_H__
#define __SCOPES_SELFUR_H__

#include "contiki-net.h"
#include "sys/ctimer.h"

#include "scopes-routing.h"
#include "scopes-types.h"

/* rime channels */
#define SELFUR_TREE_UPDATE_CHANNEL 129
#define SELFUR_ACTIVATION_CHANNEL 130
#define SELFUR_HOPCOUNT_CHANNEL 131
#define SELFUR_DATA_FLOOD_CHANNEL 132
#define SELFUR_DATA_UNICAST_CHANNEL 133
#define SELFUR_SUPPRESS_CHANNEL 134

/* selfur status definitions */
#define SELFUR_STATUS_NONE 0x0
#define SELFUR_STATUS_MEMBER 0x1
#define SELFUR_STATUS_FORWARDER 0x2
#define SELFUR_STATUS_ACTIVATE 0x4
#define SELFUR_STATUS_SUPPRESS 0x8

/* other definitions */
//#define SELFUR_FLOODING_QUEUE_TIME 1 // Message is queued this time until send, if multiple messages are sent in this periode only the newes one is sent.
#define SELFUR_FLOODING_QUEUE_TIME 2
#define SELFUR_ACTIVATION_RETRANSMISSIONS 10
#define SELFUR_MAX_ROUTING_ENTRIES 10
#define SELFUR_MAX_SCOPE_ENTRIES 10
#define SELFUR_ROUTING_ENTRY_TTL 360 // 360 seconds, it must be bigger than SELFUR_ROUTING_REFRESH_INTERVAL
//#define SELFUR_ROUTING_ENTRY_TTL 300
//#define SELFUR_ROUTING_ENTRY_TTL 5*CLOCK_SECOND
#define SELFUR_ROUTING_REFRESH_INTERVAL 120 // every 120 seconds rebuild tree
//#define SELFUR_ROUTING_REFRESH_INTERVAL CLOCK_SECOND
#define SELFUR_ACTIVATION_MAX_DELAY 1
#define SELFUR_LOCK_TIMER_DURATION 1
#define SELFUR_INFO_TIMER_DURATION 10

#define SELFUR_HAS_STATUS(r, s)                  \
  ((r)->status & s)

#define SELFUR_SET_STATUS(r, s)                  \
  ((r)->status |= s)

#define SELFUR_UNSET_STATUS(r, s)                \
  ((r)->status &= ~s)

struct routing_entry {
  struct routing_entry *next;
  rimeaddr_t root;
  rimeaddr_t next_hop;
  struct ctimer ttl_timer;
};

struct scope_entry {
  struct scope_entry *next;
  scope_id_t scope_id;
  uint8_t status;
  struct routing_entry *tree;
};

struct route_activation_msg {
  scope_id_t scope_id;
};

struct hopcount_msg {
  uint8_t max_hops;			// XXX: for hop count
};

struct route_suppress_msg {
  scope_id_t scope_id;
};

extern struct scopes_routing scopes_selfur;

/* XXX: added for TIKIDB support */
extern struct routing_entry * scopes_selfur_get_routing_list();

extern struct scope_entry * scopes_selfur_get_scopes_list();

extern bool scopes_selfur_is_forwarder(scope_id_t scope_id);

// position of current node in tree
extern uint8_t scopes_selfur_get_node_tree_height();

// max received tree height
extern uint8_t scopes_selfur_get_max_tree_height();


// returns a scope's local next hop's node address
extern rimeaddr_t scopes_selfur_get_scope_next_hop(scope_id_t scope_id);

#endif
