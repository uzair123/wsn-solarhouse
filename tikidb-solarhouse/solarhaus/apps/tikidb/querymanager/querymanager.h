#ifndef QUERYMANAGER_H
#define QUERYMANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include "contiki.h"
#include "net/rime.h"
#include "../tikidb-conf.h"
#include "../debug.h"

#include "querymanager-qidlist.h"
#include "query-types.h"
#include "../database/dbms.h"
#include "scopes.h"

#define SCOPES_COUNT_POS 3

// --------------------------------------------------------------------
// Callback variables
void (*callback_query_function)(query_object* q);
void (*callback_result_function)(uint8_t* resultdata, int size);
void (*callback_handle_error_function)(uint8_t* msg);

// --------------------------------------------------------------------
// Constructor and deconstructor
void querymanager_init(void (*callback_query)(query_object* q),
		void (*callback_result)(uint8_t* resultdata, int size),
		void (*callback_handle_error)(uint8_t* msg));
void querymanager_close();


// --------------------------------------------------------------------
// Callback methods
void querymanager_handle_error(const rimeaddr_t* receiver, uint8_t is_parent);
uint8_t querymanager_receive(uint8_t* data, int data_size, scope_id_t scope_id);
void querymanager_receive_query(uint8_t* wrapped_query, int wrapped_query_size, scope_id_t scope_id);
void querymanager_receive_result(uint8_t* data, int data_size, scope_id_t scope_id);


// --------------------------------------------------------------------
// Interface methods
void querymanager_send_query(uint8_t* wrapped_query, int wrapped_query_size);
uint8_t querymanager_send_result(uint8_t* resultdump, int size, query_object *qo);
void querymanager_stop_periodic_query(query_object* qo);
void querymanager_get_active_periodic_queries(uint8_t tabid);


// --------------------------------------------------------------------
// Other timer methods
void querymanager_exec_periodic_query(void *q_obj);
void querymanager_exec_slot_query(void *q_obj);
void querymanager_incoming_query_timeout(void *qo);


// --------------------------------------------------------------------
// Helper functions
void querymanager_destroy_query_object(query_object *qo);
void querymanager_forward_query(uint8_t *wrapped_query, int size);
void querymanager_handle_the_result(uint8_t* resultdump, int size, query_object *qo);
uint8_t querymanager_get_aggregation(query_object* qo, uint8_t tid, uint8_t aggregate_over_epoch);

void querymanager_init_a_new_query_object(query *q, query_object *qo, uint8_t *wrapped_query);
scope_id_t get_query_scope(uint8_t *wrapped_query, uint8_t nr);

void treeforming_send_children(uint8_t *data1, int size);
uint8_t treeforming_send_parent(uint8_t *data1, int size, query_object *qo);

#endif // QUERYMANAGER_H
