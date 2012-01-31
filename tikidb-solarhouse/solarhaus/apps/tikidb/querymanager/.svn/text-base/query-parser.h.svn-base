#ifndef QUERY_PARSER_H
#define QUERY_PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include "contiki.h"
#include "net/rime.h"
#include "../debug.h"
#include "../tikidb-conf.h"
#include "../tikidb-conf.h"
#include "query-types.h"

enum query_type query_parser_start(query* q, uint8_t* data, int index, int size);
void query_parser_select(query* q, uint8_t* data, int index, int size);
void query_parser_sample_period(query* q, uint8_t* data, int index, int size);

#endif // QUERY_PARSER_H
