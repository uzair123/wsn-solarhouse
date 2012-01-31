#ifndef __SCOPES_TYPES_H__
#define __SCOPES_TYPES_H__

#include "contiki.h"


/* type definitions */
typedef uint8_t scope_id_t;
typedef uint8_t subscriber_id_t;
typedef uint16_t scope_ttl_t;
typedef uint8_t scope_flags_t;
typedef uint8_t scope_status_t;
typedef uint8_t data_len_t;
typedef uint8_t msg_type_t;
typedef uint8_t msg_direction_t;
typedef uint8_t subscriber_status_t;

/* define bool */
typedef uint8_t bool;
#define TRUE 1
#define FALSE 0

#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL 1
#endif

#if DEBUG_LEVEL
#include <stdio.h>
#define PRINTF(x, ...) if (x<=DEBUG_LEVEL) printf(__VA_ARGS__)
#define PRINT_ARR(x, data, length)				\
	uint8_t *data2 = data;						\
	PRINTF(x,"[");								\
	int XX = 0;									\
	for (XX = 0; XX < length; XX++)				\
		PRINTF(x,"%u,",data2[XX]);				\
	PRINTF(x,"]\n")
#else
#define PRINTF(x, ...)
#define PRINT_ARR(x, data, length)
#endif

#endif
