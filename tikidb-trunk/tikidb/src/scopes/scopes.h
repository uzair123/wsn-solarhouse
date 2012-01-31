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
/** \brief Maximum number of parallel scopes */
#define SCOPES_MAX_SCOPES 5
/** \brief Maximum number of parallel subscribers */
#define SCOPES_MAX_SUBSCRIBER 2
#define SCOPES_TIMER_DURATION 2
/** \brief Scope id of the world scope (the universe) */
#define SCOPES_WORLD_SCOPE_ID 0
/** \brief Factor for re announcements of scopes */
#define SCOPES_REANNOUNCE_FACTOR 0.3

/** \brief Constant for indicating that an application is not subscribed */
#define SCOPES_APPLICATION_UNSUBSCRIBED 0
/** \brief Constant for indicating that an application is subscribed */
#define SCOPES_APPLICATION_SUBSCRIBED 1

/** \brief Constant for indicating that this node has no status yet for a given scope*/
#define SCOPES_STATUS_NONE 0
/** \brief Constant for indicating that this node is member of a given scope*/
#define SCOPES_STATUS_MEMBER 1
/** \brief Constant for indicating that this node is creator of a given scope*/
#define SCOPES_STATUS_CREATOR 2

/** \brief Flag for indicating that a scope doesn't have special characteristics */
#define SCOPES_FLAG_NONE 0
/** \brief Flag for indicating that a scope must be dynamically reevaluated even at non-member nodes */
#define SCOPES_FLAG_DYNAMIC 1

/** \brief Constant for message type indicating request to open a scope */
#define SCOPES_MSG_OPEN 0
/** \brief Constant for message type indicating request to send data through a scope */
#define SCOPES_MSG_DATA 1
/** \brief Constant for message type indicating request to close a scope */
#define SCOPES_MSG_CLOSE 2 

/* storage types */
/** \brief	Structure for storing information about a subscribed application */
struct scopes_subscriber {
	/** \brief Pointer to next subscriber*/
	struct scopes_subscriber *next;
	/** \brief ID of the subscriber */
	subscriber_id_t id;
	/** \brief Status of the application (subscribed/not subscribed) */
	subscriber_status_t status;
	/** \brief Pointer to application callbacks */
	struct scopes_application *application;
	/** \brief Process running the application */
	struct process *process;
};

/** \brief	Structure for storing information about a scope */
struct scope {
	/** \brief Pointer to next scope */
	struct scope *next;
	/** \brief Scope id */
	scope_id_t scope_id;
	/** \brief Scope id of parent scope */
	scope_id_t super_scope_id;
	/** \brief Subscribed application that owns (uses) this scope */
	struct scopes_subscriber *owner;
	/** \brief Usage counter (in case the application opens it more than once) */
	uint8_t use_counter;
	/** \brief Time to live of the scope */
	scope_ttl_t ttl;
	/** \brief Callback timer for invoking the handling function when the ttl expires */
	struct ctimer ttl_timer;
	/** \brief Status of this scope */
	scope_status_t status;
	/** \brief Flags of this scope */
	scope_flags_t flags;
	/** \brief Pointer to specification of the scope */
	void *specs;
	/** \brief Length of the specification */
	data_len_t spec_len;
};

/* Message types */
/** \brief	Structure defining the contents of a message requesting to open a scope */
struct scopes_msg_open {
	/** \brief Scope id of the parent scope*/
	scope_id_t scope_id;
	/** \brief Type of the message (see message types as constants) */
	msg_type_t type;
	/** \brief Scope id of the scope to open*/
	scope_id_t sub_scope_id;
	/** \brief Subscriber id of the owning application */
	subscriber_id_t owner_sid;
	/** \brief Time to live of the scope */
	scope_ttl_t ttl;
	/** \brief Flags of the scope being opened */
	scope_flags_t flags;
	/** \brief Length of the specification */
	data_len_t spec_len;
	/** followed by the byte array containing the specification */
};

/** \brief	Structure defining the contents of a message requesting to close a scope */
struct scopes_msg_close {
	/** \brief Scope id of the parent scope */
	scope_id_t scope_id;
	/** \brief Type of the message (see message types as constants) */
	msg_type_t type;
	/** \brief Scope id of the scope to close */
	scope_id_t sub_scope_id;
};

/** \brief	Structure defining the header of a message containing data to be sent through a scope */
struct scopes_msg_data {
	/** \brief Scope id of the scope through which the message should be sent */
	scope_id_t scope_id;
	/** \brief Type of the message (see message types as constants) */
	msg_type_t type;
	/** \brief Target Subscriber id */
	subscriber_id_t target_sid;
	/** \brief Direction of the message (0: to members, 1: to creator) */
	msg_direction_t to_creator;
	/** \brief Length, in bytes, of the payload */
	data_len_t data_len;
	/** \brief Address of the originating node which sent the message */
	rimeaddr_t source;
};

///* external variable declarations */
//extern struct scope world_scope;

///* external function declarations */
//void scopes_init(struct scopes_routing *routing,
//                 struct scopes_membership *membership);
//
//int scopes_register(struct subscriber *subscriber, subscriber_id_t id,
//                    struct scopes_application *application);
//
//void scopes_unregister(struct subscriber *subscriber);
//
//struct scope *scopes_open(struct subscriber *subscriber,
//                          struct scope *super_scope, scope_id_t scope_id,
//                          void *specs, data_len_t spec_len,
//                          scope_flags_t flags, scope_ttl_t ttl);
//
//void scopes_close(struct subscriber *subscriber, struct scope *scope);
//
//void scopes_send(struct subscriber *subscriber, struct scope *scope, bool to_creator,
//                 void *data, data_len_t data_len);

//void scopes_receive(struct scopes_msg_generic *gmsg);

//struct scope *scopes_lookup(scope_id_t scope_id);

#endif
