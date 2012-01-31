#ifndef QUERYRESOLVER_H
#define QUERYRESOLVER_H

#include <stdio.h>
#include <stdlib.h>
#include "contiki.h"

#include "lib/sensors.h"
#include "dev/cc2420.h"


#include "../debug.h"
#include "queryresolver-util.h"
#include "../database/dbms.h"
#include "../querymanager/querymanager.h"
//#include "../treeforming/treeforming.h"
#include "scopes.h"

// --------------------------------------------------------------------
// Callback variables
void (*callback_handle_result_function)(uint8_t* resultdump);
void (*callback_handle_error_function)(uint8_t* msg);

// --------------------------------------------------------------------
// Constructor and destructor
void queryresolver_init(void (*callback_result)(uint8_t* resultdump),
		void (*callback_error)(uint8_t* msg));
void queryresolver_close();

// --------------------------------------------------------------------
// Callback methods
void queryresolver_incoming_query(query_object* qo);
void queryresolver_incoming_result(uint8_t* resultdata, int size);
void queryresolver_handle_error(uint8_t* msg);


// --------------------------------------------------------------------
// Interface method
// void queryresolver_send_query(uint8_t* wrapped_query, int wrapped_query_size);


// --------------------------------------------------------------------
// Helper methods
void queryresolver_handle_stop_query(query_object *qo);
void queryresolver_handle_select_query(query_object *qo);
void queryresolver_handle_list_queries_query(query_object *qo);


uint8_t queryresolver_read_requested_values(int erow[],
		query_object* qo);
uint8_t queryresolver_get_requested_values_count(query_object* qo);
void queryresolver_get_requested_values(query_object* qo, uint8_t* eids);
uint8_t queryresolver_handle_conditions(int erow[],
		query_object* qo);
uint8_t queryresolver_evaluate_condition(enum attribute_type val1,
		uint16_t val2, enum comparison_operator op);
uint8_t queryresolver_evaluate_conjunction(uint8_t val1, uint8_t val2,
		enum logic_conjunction conj);



// --------------------------------------------------------------------
// Definitions for the sensortable
static uint8_t queryresolver_sensortable_p_key[] =
		{NODE_ID_TIKIDB, QUERY_ID, EPOCH};
		
static uint8_t queryresolver_sensortable_scheme[] =	{
	NODE_ID_TIKIDB, 
	QUERY_ID, 
	EPOCH, 
	CO2,
	HUMIDITY, 
	TEMPERATURE, 
	RSSI, 
};		// MOD[David]: Change in light and co2

#define QUERYRESOLVER_SENSORTABLE_P_KEY_COUNT 3
#define QUERYRESOLVER_SENSORTABLE_SCHEME_COUNT 7


#endif // QUERYRESOLVER_H
