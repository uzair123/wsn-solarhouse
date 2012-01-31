#ifndef __SCOPES_MEMBERSHIP_H__
#define __SCOPES_MEMBERSHIP_H__

#include "scopes-types.h"

struct scopes_membership {
  const char *name;
  void (* init)(void);
  int (* check)(void *specs, data_len_t spec_len);
};

#define MEMBERSHIP(name, strname, init, check)           \
  struct scopes_membership name = { strname, init, check }

#endif
