#ifndef __SCOPES_APPLICATION_H__
#define __SCOPES_APPLICATION_H__

#include "scopes-types.h"
#include "net/rime.h"

/* callbacks */
struct scopes_application {
  void (* add)(scope_id_t scope_id);
  void (* remove)(scope_id_t scope_id);
  void (* join)(scope_id_t scope_id);
  void (* leave)(scope_id_t scope_id);
  void (* recv)(scope_id_t scope_id, void *data, data_len_t data_len, bool to_creator, const rimeaddr_t *source);
};

/* external function declarations */
extern void scopes_init(struct scopes_routing *routing,
                 struct scopes_membership *membership);

extern int scopes_register(subscriber_id_t id, struct scopes_application *application);

extern void scopes_unregister(subscriber_id_t subscriber_id);

extern bool scopes_open(subscriber_id_t subscriber_id,
                          scope_id_t super_scope, scope_id_t scope_id,
                          void *specs, data_len_t spec_len,
                          scope_flags_t flags, scope_ttl_t ttl);

extern void scopes_close(subscriber_id_t subscriber_id, scope_id_t scope_id);

extern void scopes_send(subscriber_id_t subscriber_id, scope_id_t scope_id, bool to_creator,
                 void *data, data_len_t data_len);

extern bool scopes_member_of(scope_id_t scope_id);

extern bool scopes_creator_of(scope_id_t scope_id);

#endif
