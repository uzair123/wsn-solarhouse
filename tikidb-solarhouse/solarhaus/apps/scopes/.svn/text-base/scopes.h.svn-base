#ifndef __SCOPES_H__
#define __SCOPES_H__

#include "scopes-types.h"
#include "scopes-routing.h"
#include "scopes-membership.h"
#include "scopes-application.h"

/* preprocessor macros */
#define IS_WORLD_SCOPE(scope)                   \
  ((scope) == &world_scope)

#define HAS_STATUS(scope, s)                    \
  ((scope)->status & s)

#define HAS_FLAG(scope, flag)                   \
  ((scope)->flags & flag)

#define SCOPE(scope_id)                         \
  (scopes_lookup(scope_id))

#define IS_OWNER(scope, subscriber)             \
  ((scope)->owner->id == (subscriber)->id)

#define IS_SUBSCRIBED(subscriber)               \
  ((subscriber)->status == SCOPES_APPLICATION_SUBSCRIBED)

/* preprocessor constants */
#define SCOPES_MAX_SCOPES 5
#define SCOPES_TIMER_DURATION 2
#define SCOPES_WORLD_SCOPE_ID 0
#define SCOPES_REANNOUNCE_FACTOR 0.3

#define SCOPES_APPLICATION_UNSUBSCRIBED 0
#define SCOPES_APPLICATION_SUBSCRIBED 1

#define SCOPES_STATUS_NONE 0
#define SCOPES_STATUS_MEMBER 1
#define SCOPES_STATUS_CREATOR 2

#define SCOPES_FLAG_NONE 0
#define SCOPES_FLAG_DYNAMIC 1

#define SCOPES_MSG_OPEN 0
#define SCOPES_MSG_DATA 1
#define SCOPES_MSG_CLOSE 2 

/* define bool */
typedef uint8_t bool;
#define TRUE 1
#define FALSE 0

//#define SCOPES_DEBUG 1
//#define DEBUG 1
//#define DEBUG_SCOPES 1

#ifdef DEBUG
#define LOG_L(...) printf(__VA_ARGS__)
#else
#define LOG_L(...)
#endif

/* external variable declarations */
extern struct scope world_scope;

/* external function declarations */
void scopes_init(struct scopes_routing *routing,
                 struct scopes_membership *membership);

int scopes_register(struct subscriber *subscriber, subscriber_id_t id,
                    struct scopes_application *application);

void scopes_unregister(struct subscriber *subscriber);

struct scope *scopes_open(struct subscriber *subscriber,
                          struct scope *super_scope, scope_id_t scope_id,
                          void *specs, data_len_t spec_len,
                          scope_flags_t flags, scope_ttl_t ttl);

void scopes_close(struct subscriber *subscriber, struct scope *scope);

void scopes_send(struct subscriber *subscriber, struct scope *scope, bool to_creator,
                 void *data, data_len_t data_len);

void scopes_receive(struct scopes_msg_generic *gmsg);

struct scope *scopes_lookup(scope_id_t scope_id);

#endif
