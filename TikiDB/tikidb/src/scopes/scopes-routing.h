#ifndef __SCOPES_ROUTING_H__
#define __SCOPES_ROUTING_H__

#include "scopes-types.h"


struct scopes_routing {
	const char *name;
	void (* init)(void);
	void (* send)(scope_id_t scope_id, bool to_creator);
	void (* add)(scope_id_t scope, bool is_creator);
	void (* remove)(scope_id_t scope);
	void (* join)(scope_id_t scope);
	void (* leave)(scope_id_t scope);
	void (* buffer_clear)(bool to_creator);
	uint8_t* (* buffer_ptr)(bool to_creator);
	void (* buffer_setlen)(bool to_creator, uint16_t len);
};

#define ROUTING(name, strname, init, send, add, remove, join, leave, buffer_clear, buffer_ptr, buffer_setlen)     \
  struct scopes_routing name = { strname, init, send, add, remove, join, leave, buffer_clear, buffer_ptr, buffer_setlen }

#define SCOPES_MSG_CREATION 0
#define SCOPES_WORLD_SCOPE_ID 0

struct scopes_msg_generic {
	scope_id_t scope_id;
	msg_type_t type;
};


extern void scopes_receive(struct scopes_msg_generic *gmsg);

// XXX: Attention: Depends on scopes_msg_open :(
struct scopes_msg_creation {
	scope_id_t scope_id;
	msg_type_t type;
	scope_id_t sub_scope_id;
};

#endif
