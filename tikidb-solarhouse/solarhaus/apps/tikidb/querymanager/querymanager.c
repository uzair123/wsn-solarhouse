#include "querymanager.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *  C O N S T R U C T O R   A N D   D E C O N S T R U C T O R
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* --------------------------------------------------------------------
 * Is called from the layer above and initialize the querymanager.
 */
void querymanager_init(void(*callback_query)(query_object* q),
		void(*callback_result)(uint8_t* resultdata, int size),
		void(*callback_handle_error)(uint8_t* msg)) {
#ifdef DEBUG_QUERYMANAGER
	printf("querymanager_init()\n");
#endif // DEBUG_QUERYMANAGER
	// Save the adresses from the callback methods.
	callback_query_function = callback_query;
	callback_result_function = callback_result;
	callback_handle_error_function = callback_handle_error;

	querymanager_qidlist_init();
}

/* --------------------------------------------------------------------
 * Close the network connections and clear up.
 */
void querymanager_close() {
#ifdef DEBUG_QUERYMANAGER
	printf("querymanager_close()\n");
#endif // DEBUG_QUERYMANAGER
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *  C A L L B A C K   M E T H O D S
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* --------------------------------------------------------------------
 * Is called from the treeforming layer when a packet was received.
 *
 * @param timeaddr_t* sender adress of the sender
 * @param uint8_t 1 if the sender is the parent of this node
 * @param uint8_t* The message
 * @param int The size of the message
 *
 * @return 1 if an error occurs, else 0.
 */
uint8_t querymanager_receive(uint8_t* data, int data_size, scope_id_t scope_id) {
#ifdef DEBUG_QUERYMANAGER
	printf("querymanager_receive(): Received data: ");
#endif // DEBUG_QUERYMANAGER
	// Switch to the correct function dependend of the paket type.
	switch (data[0]) {
	case QUERY: {
		querymanager_receive_query(data, data_size, scope_id);
		break;
	}
	case RESULT: {
		querymanager_receive_result(data, data_size, scope_id);
		break;
	}
	default: {
		callback_handle_error_function("querymanager_receive(): The packet is invalid and will be throw away.");
		return 1;
	}
	}
	return 0;
}

/* --------------------------------------------------------------------
 * Is called from the treeforming layer when a error occured.
 *
 * @param rimeaddr_t* ID of the original receiver
 * @param uint8_t 1 if the sender is the parent of the node
 */
void querymanager_handle_error(const rimeaddr_t* receiver, uint8_t is_parent) {
#ifdef DEBUG_QUERYMANAGER
	printf("querymanager_handle_error()\n");
#endif // DEBUG_QUERYMANAGER
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *  I N T E R F A C E   M E T H O D S
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*
 * Is called from the layer above to send a query message to the network.
 *
 * @param uint8_t*	The query stream to send
 * @param int	The size of the query stream
 *
 * @return error code, else 0
 * error codes:
 * 1: memory full
 * 2: data size is to large
 * 3: max number of active packets reached
 * 4: no sequence numbers free
 * 5: QID is a duplicate
 * 6: Undefined error
 */
void querymanager_send_query(uint8_t* wrapped_query, int wrapped_query_size) {
#ifdef DEBUG_QUERYMANAGER
	printf("\nquerymanager_send_query():\n");
#endif // DEBUG_QUERYMANAGER
	// Create a new query object
	query_object *qo;
	qo = malloc(sizeof(query_object));
	if (qo == NULL) {
#ifdef DEBUG_QUERYMANAGER
		printf("querymanager_send_query(): Could not allocate memory for qo! Abort\n");
#endif // DEBUG_QUERYMANAGER
		return;
	}

	// Load the query struct from the packet's data.
	query *q = malloc(sizeof(query));
	if (q == NULL) {
#ifdef DEBUG_QUERYMANAGER
		printf("querymanager_send_query(): Could not allocate memory for q! Abort\n");
#endif // DEBUG_QUERYMANAGER
		return;
	}

	// Convert the data stream into a query struct.
	q->conditions = 0;
	q->select = 0;
	q->num_selects = 0;
	q->qid = 0;
	q->sample_period = 20;

	enum query_type type = query_parser_start(q, wrapped_query, 3 + wrapped_query[SCOPES_COUNT_POS] + 1, wrapped_query_size);
	qo->type = type;

	if (wrapped_query[SCOPES_COUNT_POS] > 0) {
		qo->scope_id = get_query_scope(wrapped_query, 1);
	} else
		qo->scope_id = 0;

	// Initialize the new query object.
	//	querymanager_init_a_new_query_object_at_root(q, qo);
	querymanager_init_a_new_query_object(q, qo, wrapped_query);
	//	qo->tree_height = get_query_height_ptr(wrapped_query)[0];

#ifdef DEBUG_QUERYMANAGER
	printf("querymanager_send_query(): Total Lifetime: %d, "
			"Actual Lifetime: %d, Sample period: %d\n",
			qo->the_query->lifetime, qo->actual_lifetime,
			qo->the_query->sample_period);
#endif // DEBUG_QUERYMANAGER
	// Save the new query object (with the qid)
	querymanager_qidlist_add(qo);

	// Save the data stream and the size of it.
	qo->wrapped_query = malloc(wrapped_query_size);
	memcpy(qo->wrapped_query, wrapped_query, wrapped_query_size);
	qo->wrapped_query_size = wrapped_query_size;

	querymanager_forward_query(wrapped_query, wrapped_query_size);

	// Start the periodic timer, if this is a select query.
	if (qo->type == SELECT) {
		querymanager_exec_periodic_query(qo);
	} else {
#ifdef DEBUG_QUERYMANAGER
		printf("querymanager_receive_query(): STOP or LIST QUERY query detected.\n");
#endif // DEBUG_QUERYMANAGER
		// This is either an STOP or LIST QUERIES query.
		// Process this query in this node immediately.
		//if(am_I_a_member(generate32BitQid(qo->query_id1, qo->query_id2)))
		callback_query_function(qo);
	}
#ifdef DEBUG_QUERYMANAGER
	printf("querymanager_send_query(): Leave with QO at %p\n", qo);
#endif // DEBUG_QUERYMANAGER
}

/* --------------------------------------------------------------------
 * Is called from the layer above to send a result message to the network.
 *
 * @param timeaddr_t* sender adress of the sender
 * @param char* The message
 * @param int The size of the message
 *
 * @return error code, else 0
 * error codes:
 * 1: memory full
 * 2: data size is to large
 * 3: max number of active packets reached
 * 4: no sequence numbers free
 */
uint8_t querymanager_send_result(uint8_t* resultdump, int size, query_object *qo) {
#ifdef DEBUG_QUERYMANAGER
	printf("querymanager::querymanager_send_result() : function entered\n");
#endif // DEBUG_QUERYMANAGER
	resultdump[0] = RESULT;
	resultdump[1] = qo->qid; //query is only one byte now (Khalid)
	resultdump[2] = qo->the_query->sample_period / (qo->wrapped_query[2] + 1); //slot length (Khalid)

#ifdef DEBUG_QUERYMANAGER
	printf("querymanager_send_result(): Send the result to the parent.\n");
#endif // DEBUG_QUERYMANAGER
	// Send the result immediately

	treeforming_send_parent(resultdump, size, qo);

	// Free the memory from the old object.
	if (qo->temporary_table_id != 0) {
		dbms_drop_table(qo->temporary_table_id);
		qo->temporary_table_id = 0;
	}
	
	return 0;
}

/* --------------------------------------------------------------------
 * Stops a active periodic query. Is called from the layer above.
 * @param	query_object*	Pointer to the query object with the periodic
 * 							query to stop.
 */
void querymanager_stop_periodic_query(query_object* qo) {

#ifdef DEBUG_QUERYMANAGER
	printf("querymanager_stop_periodic_query()\n\r");
#endif // DEBUG_QUERYMANAGER
	qo->stop_query_flag = 1;
}

/* --------------------------------------------------------------------
 * Get all active periodic queries and write there QID in a new table,
 * which ID is commit as parameter. Is called from the layer above.
 *
 * @param uint8_t ID from a table to write the active periodic
 * 				  queries into it. The table have to be the scheme
 * 				  {NODE_ID_TIKIDB, QUERY_ID_1, QUERY_ID_2}.
 */
void querymanager_get_active_periodic_queries(uint8_t tabid) {

#ifdef DEBUG_QUERYMANAGER
	printf("querymanager_get_active_periodic_queries():\n\r");
#endif // DEBUG_QUERYMANAGER
	// The scheme for the return table.
	uint8_t scheme[] = { NODE_ID_TIKIDB, QUERY_ID };

	// Go through the list of query objects.
	struct qid_list_element *e = querymanager_qidlist_get_list();

	while (e) {

		// Get the current query and add a new entry in the table.
		int new_row[] = { (int) rimeaddr_node_addr.u8[0]
				+ ((int) rimeaddr_node_addr.u8[1] << 8), e->qo->qid };

#ifdef DEBUG_QUERYMANAGER
		if (dbms_add_entry(tabid, scheme, new_row, 2) == 1)
		printf("querymanager_get_active_periodic_queries(): WARNING: Couldn't add entry in table: %u\n\r", tabid);
#else
		dbms_add_entry(tabid, scheme, new_row, 2);
#endif // DEBUG_QUERYMANAGER
		e = (struct qid_list_element*) e->next;
	} // while

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *  O T H E R   T I M E R   M E T H O D S
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void querymanager_exec_periodic_query(void *q_obj) {
	query_object* qo = (query_object *) q_obj;

	// If this query should be stopped, deativate the timer and
	// destroy the objects of this query.
	if (qo->stop_query_flag == 1) {
#ifdef DEBUG_QUERYMANAGER
		printf("querymanager_exec_periodic_query(): Destroy this query.\n");
#endif // DEBUG_QUERYMANAGER
		ctimer_stop(&qo->periodic_timer);
		ctimer_stop(&qo->in_slot_timer);

		querymanager_destroy_query_object(qo);
		printf("querymanager::querymanager_exec_periodic_query() : stopping query\n");

		return;
	}

	// One more executed... Add to the actual duration of the query's
	//  execution in seconds the time that were gone until this execution.
	qo->actual_lifetime += qo->the_query->sample_period;

#ifdef DEBUG_QUERYMANAGER
	printf("querymanager_exec_periodic_query(): Total Lifetime: %d, "
			"Actual Lifetime: %d, Sample period: %d\n",
			qo->the_query->lifetime, qo->actual_lifetime,
			qo->the_query->sample_period);
#endif // DEBUG_QUERYMANAGER

	// set the timers
	uint8_t slot = qo->the_query->sample_period / (qo->wrapped_query[2] + 1);

	ctimer_set(&qo->periodic_timer, CLOCK_SECOND
			* qo->the_query->sample_period,
			&querymanager_exec_periodic_query, qo);

	clock_time_t delay = (slot) * (qo->wrapped_query[2]
			- get_node_tree_height()) * CLOCK_SECOND + random_rand()
			% (slot * CLOCK_SECOND / 2);

	ctimer_set(&qo->in_slot_timer, delay, &querymanager_exec_slot_query, qo);

#ifdef DEBUG_QUERYMANAGER
	printf("querymanager::querymanager_exec_periodic_query() : posted more timers at %u and %u, actual lifetime: %u, lifetime: %u\n", qo->the_query->sample_period, delay / CLOCK_SECOND, qo->actual_lifetime, qo->the_query->lifetime);
#endif // DEBUG_QUERYMANAGER
}

void querymanager_exec_slot_query(void *q_obj) {
#ifdef DEBUG_QUERYMANAGER
	printf("querymanager::querymanager_exec_slot_query() : function entered\n");
#endif // DEBUG_QUERYMANAGER
	// Define the type of the void pointer.
	query_object* qo = (query_object *) q_obj;
	callback_query_function(qo);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *  H E L P E R   M E T H O D S
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* --------------------------------------------------------------------
 * Remove a query object and all assoziated structs from the memory.
 * @param query_object*	The query object with the query to send.
 */
void querymanager_destroy_query_object(query_object *qo) {
#ifdef DEBUG_QUERYMANAGER
	printf("querymanager_destroy_query_object()\n");
#endif // DEBUG_QUERYMANAGER
	// Free the allocated memory for the list with the SELECT statements.
	if (qo->the_query->select != NULL) {
		// It exists at least one element.

		free(qo->the_query->select);
		qo->the_query->num_selects = 0;

	} // if

	// Free the allocated memory for the list with the WHERE statements.
	if (qo->the_query->conditions != NULL) {
		// It exists at least one element.

		if (qo->the_query->conditions->next_condition == NULL) {

			// It esists exactly one elements.
			free(qo->the_query->conditions);

		} else {
			// It exists more than one element.
			struct condition *c = qo->the_query->conditions;
			struct condition *last_c = NULL;

			while (c) {

				// If this is not the first iteration, free the last pointer.
				if (last_c != NULL) {
					free(last_c);
				}

				// If the list isn't at the end, save the actual pointer.
				if (c->next_condition != NULL) {
					last_c = c;
				}

				c = (struct condition*) c->next_condition;
			}
			free(last_c);

		} // else
	} // if

	free(qo->the_query);
	free(qo->wrapped_query);
	querymanager_qidlist_remove(qo);
	free(qo);
}

/* --------------------------------------------------------------------
 * Forward an (incoming) query to the childrens.
 * @param timeaddr_t* sender adress of the sender
 * @param query_object*	The query object with the query to send.
 */
void querymanager_forward_query(uint8_t *wrapped_query, int wrapped_query_size) {

#ifdef DEBUG_QUERYMANAGER
	printf("querymanager_forward_query()\n");
#endif // DEBUG_QUERYMANAGER
	treeforming_send_children(wrapped_query, wrapped_query_size);
}

/* --------------------------------------------------------------------
 * Handle an incoming query.
 * @param timeaddr_t* sender adress of the sender
 * @param uint8_t 1 if the sender is the parent of this node
 * @param uint8_t* The packet's wrapped_query
 * @param int The size of the message
 */
void querymanager_receive_query(uint8_t* wrapped_query, int wrapped_query_size, scope_id_t scope_id) {
	uint8_t qid = wrapped_query[1];
#ifdef DEBUG_QUERYMANAGER
	printf("querymanager_receive_query(): Received query\n\r");
	printf("querymanager::querymanager_receive_query() :: Received query with id %u\n", qid);
	printf("querymanager_receive_query(): Received packet has QID ID:4%u\n", qid);
#endif // DEBUG_QUERYMANAGER
	if (querymanager_qidlist_contains_qid(qid)) {
#ifdef DEBUG_QUERYMANAGER
		printf("querymanager_receive_query(): Packet is known and will be throw away.\n");
#endif // DEBUG_QUERYMANAGER
		// The query has been proceeded already.
		return;
	}

	// Else, the query is new.

	// Create a new QID object.
	query_object *qo;
	qo = malloc(sizeof(query_object));
	if (qo == NULL) {
#ifdef DEBUG_QUERYMANAGER
		printf("querymanager_receive_query(): Could not allocate memory for qo. Abort.\n");
#endif // DEBUG_QUERYMANAGER
		return;
	}

	// Convert the data stream into a query struct.
	query *q = malloc(sizeof(query));
	if (q == NULL) {
#ifdef DEBUG_QUERYMANAGER
		printf("querymanager_send_query(): Could not allocate memory for q! Abort\n");
#endif // DEBUG_QUERYMANAGER
		return;
	}
	
	q->conditions = 0;
	q->select = 0;
	q->num_selects = 0;
	q->qid = 0;
	q->sample_period = 20;

	enum query_type type = query_parser_start(q, wrapped_query, 3 + wrapped_query[SCOPES_COUNT_POS] + 1, wrapped_query_size);

	qo->type = type;

	if (wrapped_query[SCOPES_COUNT_POS] > 0) {
		qo->scope_id = get_query_scope(wrapped_query, 1);
#ifdef DEBUG_QUERYMANAGER
		printf("querymanager::querymanager_receive_query() :: query arrived through scope, setting it to %u\n", qo->scope_id);
#endif // DEBUG_QUERYMANAGER		
	} else
		qo->scope_id = 0;
#ifdef DEBUG_QUERYMANAGER
	printf("querymanager::querymanager_receive_query() :: query was new, 'query' built, op was of type %u\n", qo->type);
#endif // DEBUG_QUERYMANAGER
	// Initialize the new query object.
	querymanager_init_a_new_query_object(q, qo, wrapped_query);
	//	qo->tree_height = get_query_height_ptr(wrapped_query)[0];

	// Save the data stream and the size of it.
	qo->wrapped_query = malloc(wrapped_query_size);
	memcpy(qo->wrapped_query, wrapped_query, wrapped_query_size);
	qo->wrapped_query_size = wrapped_query_size;

	// Copy the received QID
	qo->qid = qid;

	// Save the new query object.
	querymanager_qidlist_add(qo);

	// Start the periodic timer, if this is a select query.
	if (qo->type == SELECT) {
#ifdef DEBUG_QUERYMANAGER
		printf("querymanager_receive_query(): Starting periodic timer.\n");
#endif // DEBUG_QUERYMANAGER
		querymanager_exec_periodic_query(qo);

	} else {
#ifdef DEBUG_QUERYMANAGER
		printf("querymanager_receive_query(): STOP or LIST QUERY query detected.\n");
#endif // DEBUG_QUERYMANAGER
		callback_query_function(qo);
	}
}

/* --------------------------------------------------------------------
 * Handle an incoming result.
 * @param timeaddr_t* sender adress of the sender
 * @param uint8_t 1 if the sender is the parent of this node
 * @param uint8_t* The message
 * @param int The size of the message
 */
void querymanager_receive_result(uint8_t* data, int size, scope_id_t scope_id) {
#ifdef DEBUG_QUERYMANAGER
	printf("querymanager_receive_result(): Received result is from scopeid %u\n\r", scope_id);
#endif // DEBUG_QUERYMANAGER

	query_object *qo = querymanager_qidlist_get_by_qid(data[1]);
	uint16_t epoch = db_dump_get_epoch(data);

	// Check to see whether this node is simply a forwarder:
	if (is_forwarder(scope_id) && !HAS_STATUS(scopes_lookup(scope_id),SCOPES_STATUS_MEMBER)) {

		uint8_t slot_length = 0;
		if (qo == NULL) {
			qo = (query_object *) malloc(sizeof(query_object));
			query *q = (query *) malloc(sizeof(query));
			if ((!qo) || (!q)) {
#ifdef DEBUG_QUERYMANAGER
				printf("querymanager_receive_result(): Could not allocate memory for qo or q! Abort\n");
#endif // DEBUG_QUERYMANAGER
				return;
			}
			querymanager_init_a_new_query_object(q, qo, NULL);
			qo->qid = data[1];
			qo->temporary_table_id = 0;
			qo->type = SELECT;
			qo->scope_id = scope_id;
			slot_length = data[2];
			// Save the new query object.
			querymanager_qidlist_add(qo);
			printf("querymanager_receive_result(): No query object for the received result, assuming forwarder (trigger in %u)\n", slot_length);
		}
		ctimer_set(&qo->in_slot_timer, CLOCK_SECOND * slot_length, &querymanager_exec_slot_query, qo);
	} else if (qo != NULL)
		epoch = qo->actual_lifetime / qo->the_query->sample_period;

#ifdef DEBUG_QUERYMANAGER
	printf("querymanager_receive_result(): It exists a query object at address %p.\n", qo);
#endif // DEBUG_QUERYMANAGER

	if (epoch == db_dump_get_epoch(data)) {
		querymanager_handle_the_result(data, size, qo);
	} else {
#ifdef DEBUG_QUERYMANAGER
		printf("querymanager_receive_result(): Discarding %u received results from epoch %u, this node's epoch is %u\n", db_dump_get_num_entries(data), db_dump_get_epoch(data), epoch);
#endif // DEBUG_QUERYMANAGER
	}
}

/* --------------------------------------------------------------------
 * Handle an incoming result.
 * @param data* The message
 * @param int The size of the message
 * @param query_object The query object
 */
void querymanager_handle_the_result(uint8_t* resultdump, int size, query_object *qo) {
#ifdef DEBUG_QUERYMANAGER
	printf("querymanager_handle_the_result()\n");
#endif // DEBUG_QUERYMANAGER

	int received_result_table = dbms_load_dump(resultdump);

#ifdef DEBUG_QUERYMANAGER
	if (received_result_table < 0) {
		printf("querymanager_handle_the_result(): Error at dbms_load_dump() with return %d. Dump: ", received_result_table);
		int i;
		for(i=0;i<size;i++) printf("%c", resultdump[i]);
		printf("\n");
		return;
	}
#endif // DEBUG_QUERYMANAGER

	// Bring the new with the current table together.
	if (qo->temporary_table_id != 0) {

#ifdef DEBUG_QUERYMANAGER_TABLES
		printf("querymanager_handle_the_result(): dbms_union(%u,%u)\n", qo->temporary_table_id, received_result_table);
		dbms_printf_table(qo->temporary_table_id);
		dbms_printf_table(received_result_table);
#endif // DEBUG_QUERYMANAGER_TABLES

		// Execute the union
		qo->temporary_table_id = dbms_union(qo->temporary_table_id, received_result_table);

#ifdef DEBUG_QUERYMANAGER_TABLES
		if (qo->temporary_table_id != 0) {
			printf("querymanager_handle_the_result(): Union result of %u:\n", qo->temporary_table_id);
			dbms_printf_table(qo->temporary_table_id);
		}
		printf("querymanager::querymanager_handle_the_result() : there already was a temporary table with id %u\n", qo->temporary_table_id);
#endif // DEBUG_QUERYMANAGER_TABLES
		// Remove the temporary table.
		dbms_drop_table(received_result_table);

	} else {
		qo->temporary_table_id = received_result_table;
#ifdef DEBUG_QUERYMANAGER
		printf("querymanager::querymanager_handle_the_result() : there was no temporary table, created one with id %u\n", qo->temporary_table_id);		
		printf("querymanager_handle_the_result(): First result table %u:\n", qo->temporary_table_id);
		dbms_printf_table(qo->temporary_table_id);
#endif // DEBUG_QUERYMANAGER
	}

	//dbms_printf_table(qo->temporary_table_id);

#ifdef DEBUG_QUERYMANAGER
	printf("querymanager_handle_the_result(): Delete table %d.\n", received_result_table);
#endif // DEBUG_QUERYMANAGER
}

/* --------------------------------------------------------------------
 * Returns the first (!) aggregation type from a query.
 * @param query_object The query object
 * @param uint8_t	ID of the temporary table with the results.
 * @param uint8_t	If 1, then it will be grouped over the epoch row;
 *                  else over the row in s->value (also the structure).
 * @return uint8_t	The id of the old table
 */
uint8_t querymanager_get_aggregation(query_object* qo, uint8_t tid, uint8_t aggregate_over_epoch) {
#ifdef DEBUG_QUERYMANAGER
	printf("querymanager_get_aggregation()\n");
#endif // DEBUG_QUERYMANAGER
	return tid;
}

/* --------------------------------------------------------------------
 * Create a new query object with the query from the parameter
 *
 * @param 	query			Pointer to the query
 * @param	query_object	Pointer to the new query objects
 */
// CHRIS qid added
// void querymanager_init_a_new_query_object(query *q, query_object *qo) {
void querymanager_init_a_new_query_object(query *q, query_object *qo,
		uint8_t *wrapped_query) {

#ifdef DEBUG_QUERYMANAGER
	printf("querymanager_init_a_new_query_object()\n");
#endif // DEBUG_QUERYMANAGER
	// Link the query to the query object
	qo->the_query = q;

	if (wrapped_query != NULL) {
		qo->qid = wrapped_query[1];
		qo->tree_height = wrapped_query[2];
	}

	// Initialize the temporary_table_id
	qo->temporary_table_id = 0;

	// Initialize the stop query flag.
	qo->stop_query_flag = 0;

	// Initialize the exec counter.
	qo->actual_lifetime = 0;

}


scope_id_t get_query_scope(uint8_t *wrapped_query, uint8_t nr) {
	if (nr <= wrapped_query[SCOPES_COUNT_POS])
		return wrapped_query[SCOPES_COUNT_POS + nr];
	else
		return 0;
}
