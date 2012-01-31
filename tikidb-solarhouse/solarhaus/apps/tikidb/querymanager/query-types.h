#ifndef QUERY_TYPES_H
#define QUERY_TYPES_H

#include <stdio.h>
#include <stdlib.h>
#include "contiki.h"
#include "net/rime.h"

#include "../debug.h"
#include "../tikidb-conf.h"
#include "scopes.h"

/*****************************************************************/
/* Enumeration list for the type.                                */
/*****************************************************************/

//// Sum of the queries from this node.
//static int query_types_query_object_count = 0;

// Value for a lifetime of "infinity" for a periodic query.
#define INFINITY_LIFETIME		6000

/*****************************************************************/
/* Enumeration list for the type.                                */
/*****************************************************************/
enum query_type {
	STOP = 6, SELECT = 1, LIST_QUERIES = 7
} query_types;

/*****************************************************************/
/* Enumeration list for TikiDB message types.                    */
/*****************************************************************/
enum tikidb_msg_type {
	QUERY = 0, RESULT = 1
} tikidb_msg_types;

/*****************************************************************/
/* General keywords                                              */
/*****************************************************************/

#define	SAMPLE_PERIOD			4
#define LIFETIME				5

/*****************************************************************/
/* Enumeration list for the sensors and attributes.              */
/*****************************************************************/
enum attribute_type {
	NODE_ID_TIKIDB = 40,
	CO2 = 41,					// MOD[David]: Second light value changed to CO2
	HUMIDITY = 42,
	TEMPERATURE = 43,
	RSSI = 44,
	QUERY_ID = 60,
	EPOCH = 62
} attribute_types;

/*****************************************************************/
/* Aggregation operators                                         */
/*****************************************************************/
enum aggregation_type {
	NONE = 11, MAX = 12, MIN = 13, AVG = 14, COUNT = 15, SUM = 16
} aggregation_types;

struct select_col {
	enum aggregation_type aggregation;
	enum attribute_type value;
//	struct select_col *next_select;
};

/*****************************************************************/
/* comparison operators                                          */
/*****************************************************************/
enum comparison_operator {
	SMALLER = 21,
	SMALLER_EQUAL = 22,
	GREATER = 23,
	GREATER_EQUAL = 24,
	EQUAL = 25,
	NOT_EQUAL = 26
} comparison_operators;

/*****************************************************************/
/* Conditions                                                    */
/*****************************************************************/
enum logic_conjunction {
	AND = 31, OR = 32
} logic_conjunctions;

struct condition {
	enum attribute_type val1;
	enum comparison_operator op;
	uint16_t val2;
	enum logic_conjunction conjunction;
	struct condition *next_condition;
};

/*****************************************************************/
/* The whole thing                                               */
/*****************************************************************/
typedef struct {

	// ID for the query which is to be stopped.
	uint8_t qid;

	// Pointer to the first select condition. The next select
	//  condition are linked from the first entry.
	struct select_col *select;

	// number of select_col entries
	uint8_t num_selects;

	// Pointer to the first where condition. The next where
	//  condition are linked from the first entry.
	struct condition *conditions;

	// Value for the period repeat.
	uint16_t sample_period;

} query;

/*****************************************************************/
/* A query_object contains a query and assoziated data such a qid*/
/*****************************************************************/

typedef struct {

	// Unique ID of a query.
	// Format (32 Bit): <NODE_ID_TIKIDB_1_0><NODE_ID_TIKIDB_1_1><query_count>
	//					8          8          16
	uint8_t qid;

	// The id of the scope through which this query arrived to this node
	scope_id_t scope_id;

	// The query as received
	uint8_t *wrapped_query;

	// The size of the wrapped query
	uint8_t wrapped_query_size;

	// The query structure
	query *the_query;

	// Specifies the type of the query.
	enum query_type type;

	// ID of the temporary table, in which the datas are collected
	// until the time out.
	uint8_t temporary_table_id;

	//	// The sum of the remaining results from the node's children.
	//	int remaining_results;

	// The tree height used for this query;
	uint8_t tree_height;

	// Timer
	struct ctimer in_slot_timer;

	// The Timer for the periodic executions of a periodic query.
	struct ctimer periodic_timer;

	// Save how long the query is executing already. (In seconds)
	int actual_lifetime;

	// If this is true, the query should be stopped at the next time.
	uint8_t stop_query_flag;

} query_object;

#endif // QUERY_TYPES_H
