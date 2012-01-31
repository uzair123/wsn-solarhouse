#ifndef __SCOPES_APPLICATION_H__
#define __SCOPES_APPLICATION_H__

#include "scopes-types.h"

struct scopes_application {
  void (* add)(struct scope *scope);
  void (* remove)(struct scope *scope);
  void (* join)(struct scope *scope);
  void (* leave)(struct scope *scope);
  void (* recv)(struct scope *scope, void *data, data_len_t data_len);
};

#endif
