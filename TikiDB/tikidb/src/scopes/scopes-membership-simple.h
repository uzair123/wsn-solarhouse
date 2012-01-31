#ifndef __SCOPES_MEMBERSHIP_SIMPLE_H__
#define __SCOPES_MEMBERSHIP_SIMPLE_H__

#include "scopes-membership.h"

/* preprocessor macros */
#define MEMBERSHIP_NO_MORE_INPUT -1

#define IS_VALID(code)                          \
  ((code) >= 0 && (code) < sizeof(parser)/sizeof(parse_func))

/* pointer to parser function type */
typedef int (* parse_func)(void);

/* op codes */
enum opcodes { OPERATOR_AND,
               OPERATOR_OR,
               OPERATOR_NOT,
               PREDICATE_EQ,
               PREDICATE_NEQ,
               PREDICATE_LT,
               PREDICATE_GT,
               PREDICATE_LET,
               PREDICATE_GET,
               UINT8_VALUE,
               UINT16_VALUE,
               INT8_VALUE,
               INT16_VALUE,
               REPOSITORY_VALUE,
			   OPERATOR_ADD,
			   OPERATOR_SUB,
			   OPERATOR_DIV,
			   OPERATOR_MULT,
			   OPERATOR_MOD};

extern struct scopes_membership simple_membership;

#endif
