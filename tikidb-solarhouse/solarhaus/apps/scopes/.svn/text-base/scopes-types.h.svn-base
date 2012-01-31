#ifndef __SCOPES_TYPES_H__
#define __SCOPES_TYPES_H__

#include "contiki.h"
#include "ctimer.h"

/* type definitions */
typedef uint8_t scope_id_t;
typedef uint8_t subscriber_id_t;
typedef uint8_t scope_ttl_t;
typedef uint8_t scope_flags_t;
typedef uint8_t scope_status_t;
typedef uint8_t data_len_t;
typedef uint8_t msg_type_t;
typedef uint8_t msg_direction_t;
typedef uint8_t subscriber_status_t;

/* storage types */
struct subscriber {
  struct subscriber *next;
  subscriber_id_t id;
  subscriber_status_t status;
  struct scopes_application *application;
  struct process *process;
};

struct scope {
  struct scope *next;
  scope_id_t scope_id;
  scope_id_t super_scope_id;
  struct subscriber *owner;
  scope_ttl_t ttl;
  struct ctimer ttl_timer;
  scope_status_t status;
  scope_flags_t flags;
  void *specs;
  data_len_t spec_len;
};

/* message types */
struct scopes_msg_open {
  scope_id_t scope_id;
  msg_type_t type;
  scope_id_t sub_scope_id;
  subscriber_id_t owner_sid;
  scope_ttl_t ttl;
  scope_flags_t flags;
  data_len_t spec_len;
};

struct scopes_msg_close {
  scope_id_t scope_id;
  msg_type_t type;
  scope_id_t sub_scope_id;
};

struct scopes_msg_data {
  scope_id_t scope_id;
  msg_type_t type;
  subscriber_id_t target_sid;
  msg_direction_t to_creator;
  data_len_t data_len;
};

struct scopes_msg_generic {
  scope_id_t scope_id;
  msg_type_t type;
};

#endif
