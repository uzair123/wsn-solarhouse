#ifndef __SCOPES_ROUTING_H__
#define __SCOPES_ROUTING_H__

#include "scopes-types.h"

struct scopes_routing {
  const char *name;
  void     (* init)(void);
  void     (* send)(struct scopes_msg_generic *gmsg);
  void     (* add)(struct scope *scope);
  void     (* remove)(struct scope *scope);
  void     (* join)(struct scope *scope);
  void     (* leave)(struct scope *scope);
  void     (* buffer_clear)(void);
  uint8_t* (* buffer_ptr)(void);
  void     (* buffer_setlen)(uint16_t len);
};

#define ROUTING(name, strname, init, send, add, remove, join, leave, buffer_clear, buffer_ptr, buffer_setlen)     \
  struct scopes_routing name = { strname, init, send, add, remove, join, leave, buffer_clear, buffer_ptr, buffer_setlen }

#endif
