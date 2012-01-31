#ifndef __SCOPES_MEMBERSHIP_H__
#define __SCOPES_MEMBERSHIP_H__

#include "scopes-types.h"

struct scopes_membership {
  const char *name;
  void (* init)(void);
  uint8_t (* check)(uint8_t *specs, uint8_t num);
};

#define MEMBERSHIP(name, strname, init, check)           \
  struct scopes_membership name = { strname, init, check }

extern struct scopes_membership simple_membership;
  
#endif
