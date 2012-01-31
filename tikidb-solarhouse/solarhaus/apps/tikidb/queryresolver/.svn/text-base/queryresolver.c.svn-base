#include "queryresolver.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *  C O N S T R U C T O R   A N D   D E C O N S T R U C T O R
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* --------------------------------------------------------------------
 * Is called from the layer above and initialize the queryresolver.
 *
 * @param void*	Callback for the result
 * @param void* Callback for the error handler
 */
void queryresolver_init(void(*callback_result)(uint8_t* resultdump),
		void(*callback_error)(uint8_t* msg)) {
#ifdef DEBUG_QUERYRESOLVER
	printf("queryresolver_init()\n");
#endif // DEBUG_QUERYRESOLVER
	// Calling the querymanager to initialize them.
	querymanager_init(queryresolver_incoming_query,
			queryresolver_incoming_result, queryresolver_handle_error);

	// Save the adress of the callback method.
	callback_handle_result_function = callback_result;
	callback_handle_error_function = callback_error;

	// Initialize the Database management system.
	dbms_init();

	// Initialize the sensors.
	// sht11_init();				// MOD[David]: No longer available
	tmp102_init();					// MOD[David]: New temperature init
//	light_sensor.configure(SENSORS_ACTIVE,1);	// MOD[David]: New light init
	humidity_sensor.configure(SENSORS_ACTIVE,1);// MOD[David]: New humidity init
	sensorstation_init();				// MOD[David]: New sensorstation
	cc2420_init();
//	SENSORS_ACTIVATE(acc_sensor);
	//	SENSORS_ACTIVATE(sht11_sensor);
	// SENSORS_ACTIVATE(light_sensor);			// MOD[David]: Old light init
	// SENSORS_ACTIVATE(battery_sensor);		// MOD[David]: Old battery init
}

/* --------------------------------------------------------------------
 * Deconstruktor
 */
void queryresolver_close() {
#ifdef DEBUG_QUERYRESOLVER
	printf("queryresolver_close()\n");
#endif // DEBUG_QUERYRESOLVER
	querymanager_close();
	dbms_close();
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 
 *  C A L L B A C K   M E T H O D S
 * 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* --------------------------------------------------------------------
 * Takes a query and resolve them by calling querymanager_send_result
 * in a function that will be invoked here.
 *
 * @param query_object*	Pointer to the query object of the current query.
 */
void queryresolver_incoming_query(query_object* qo) {

#ifdef DEBUG_QUERYRESOLVER
	printf("queryresolver_incoming_query(): QID = %u\n", qo->the_query->qid);
#endif // DEBUG_QUERYRESOLVER
	// Determine the type of the query an switch to the
	//  handler function for this type.
	switch (qo->type) {

	case STOP: {
		queryresolver_handle_stop_query(qo);
		break;
	}

	case SELECT: {
		queryresolver_handle_select_query(qo);
		break;
	}

	case LIST_QUERIES: {
		queryresolver_handle_list_queries_query(qo);
		break;
	}

	default: {
#ifdef DEBUG_QUERYRESOLVER
		printf("queryresolver_incoming_query(): Querytype is invalid; Abort\n");
#endif // DEBUG_QUERYRESOLVER
	}

	} // switch

}

/* --------------------------------------------------------------------
 * Only for the root node: Takes a final result and forward it to
 * the layer above.
 *
 * @param uint8_t*	Result as a database dump
 */
void queryresolver_incoming_result(uint8_t* resultdata, int size) {

#ifdef DEBUG_QUERYRESOLVER
	printf("queryresolver_incoming_result()\n");

	/* CHRIS
	 if (treeforming_is_root() != 1) {
	 printf("queryresolver_incoming_result(): WARNING: This node isn't the root, but it has got a result!\n");
	 }*/

#endif // DEBUG_QUERYRESOLVER
	unsigned int resultdump_size;
	uint8_t* resultdump;

	int tmp_dump_table = dbms_load_dump(resultdata);

#ifdef DEBUG_QUERYMANAGER
	if (tmp_dump_table < 0) {
		printf("queryresolver_remove_empty_rows(): Error at dbms_load_dump() with return %d. Dump: ", tmp_dump_table);
		int i;
		for(i=0;i<size;i++) printf("%c", resultdata[i]);
		printf("\n");
		return;
	}
#endif // DEBUG_QUERYMANAGER
	unsigned int rid_scheme_size = 0;
	uint8_t *rid_scheme = dbms_get_table_scheme(tmp_dump_table,
			&rid_scheme_size);
	int empty_row[] = { 0, 0, 0 };

	// Search an "Empty"-Row and delete them.
	if (dbms_has_entry(tmp_dump_table, rid_scheme, empty_row, 3) != -1) {
		// It exists an empty row.

		if (dbms_delete_entry(tmp_dump_table, rid_scheme, empty_row, 3) != 0) {
#ifdef DEBUG_QUERYRESOLVER
			printf("queryresolver_remove_empty_rows(): WARNING: Could'nt delete an empty entry row!\n");
#endif // DEBUG_QUERYRESOLVER
		}

	}

	resultdump = dbms_get_dump(tmp_dump_table, &resultdump_size, NULL, NULL);

#ifdef DEBUG_QUERYRESOLVER
	if (resultdump == NULL)
	printf("queryresolver_remove_empty_rows(): WARNING: Couldn't get dump from table %u!\n", tmp_dump_table);
#endif // DEBUG_QUERYRESOLVER
	dbms_drop_table(tmp_dump_table);

	// Forward the final result to the layer above.
	callback_handle_result_function(resultdump);

	free(resultdump);

}

/* --------------------------------------------------------------------
 * Only for the root node: Takes a error message and forward it to
 * the layer above.
 *
 * @param uint8_t*	Error message
 */
void queryresolver_handle_error(uint8_t* msg) {

#ifdef DEBUG_QUERYRESOLVER
	printf("queryresolver_handle_error(): %s\n", msg);

	/* CHRIS
	 if (treeforming_is_root() != 1) {
	 printf("queryresolver_handle_error(): WARNING: This node isn't the root, but it has got an error message: %s\n", msg);
	 } */

#endif // DEBUG_QUERYRESOLVER
	// Forward the error message to the layer above.
	queryresolver_handle_error(msg);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *  I N T E R F A C E   M E T H O D
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

///* --------------------------------------------------------------------
// * Only for the root node: Sends a new query into the network by
// * forwarding to the querymanager. The result will come by invoking
// * the result callback function.
// *
// * @param uint8_t*	wrapped_query
// * @param int	size of the wrapped_query
// */
//void queryresolver_send_query(uint8_t* wrapped_query, int wrapped_query_size) {
//	#ifdef DEBUG_QUERYRESOLVER
//	printf("queryresolver_send_query()\n");
//	#endif // DEBUG_QUERYRESOLVER
//
//	// Forward the query to the Querymanager.
//	querymanager_send_query(wrapped_query, wrapped_query_size);
//}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *  H E L P E R   M E T H O D S
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* --------------------------------------------------------------------
 * Handle a query from the type STOP.
 *
 * @param query_object*	Pointer to the query object
 */
void queryresolver_handle_stop_query(query_object *qo) {
#ifdef DEBUG_QUERYRESOLVER
	printf("queryresolver_handle_stop_query()\n");
#endif // DEBUG_QUERYRESOLVER

	// Get the query object with the ID
	query_object* stop_qo = querymanager_qidlist_get_by_qid(qo->the_query->qid);

	// Call the routine in the querymanager to stop the query there.
	if (stop_qo != NULL) {
		querymanager_stop_periodic_query(stop_qo);
	} else {
#ifdef DEBUG_QUERYRESOLVER
		printf("queryresolver_handle_stop_query(): WARNING: Couldn't stop a query with QID %d because it doesn't exists such a query.\n", qo->the_query->qid);
#endif // DEBUG_QUERYRESOLVER
	}

	// Remove this qo from the list.
	querymanager_qidlist_remove(qo);
	ctimer_stop(&qo->periodic_timer);
	ctimer_stop(&qo->in_slot_timer);

	// TODO Send the results that are available to the layer above.
}

/* --------------------------------------------------------------------
 * Handle a query from the type SELECT.
 *
 * @param query_object*	Pointer to the query object
 */
void queryresolver_handle_select_query(query_object *qo) {

#ifdef DEBUG_QUERYRESOLVER
	printf("queryresolver_handle_select_query()\n");
#endif // DEBUG_QUERYRESOLVER
	// 1st: do sensing iff node is member of the scope
	uint8_t *resultdump = NULL;

	unsigned int size;

	if (HAS_STATUS(scopes_lookup(qo->scope_id), SCOPES_STATUS_MEMBER)) {
		// generate result dump out of own sensor data + temporary data
		if (qo->the_query != NULL) {
			// Create a new temporary sensortable
			uint stabid;

#ifdef DEBUG_QUERYRESOLVER
			if ((stabid = dbms_create_table(
									queryresolver_sensortable_scheme,
									QUERYRESOLVER_SENSORTABLE_SCHEME_COUNT,
									queryresolver_sensortable_p_key,
									QUERYRESOLVER_SENSORTABLE_P_KEY_COUNT)) < 0) {
				printf("queryresolver_handle_select_query(): WARNING: Couldn't create the sensortable! Abort.\n");
				return;
			}
#else
			stabid = dbms_create_table(queryresolver_sensortable_scheme,
					QUERYRESOLVER_SENSORTABLE_SCHEME_COUNT,
					queryresolver_sensortable_p_key,
					QUERYRESOLVER_SENSORTABLE_P_KEY_COUNT);
#endif // DEBUG_QUERYRESOLVER
			// Initialize a new row
			int i;
			int erow[QUERYRESOLVER_SENSORTABLE_SCHEME_COUNT];
			for (i = 0; i < QUERYRESOLVER_SENSORTABLE_SCHEME_COUNT; i++)
				erow[i] = 0;

			// Get the actual sensor values for each SELECT-Statement
			queryresolver_read_requested_values(erow, qo);

			// Check all conditions. If they evaluate to false, delete
			//  all values from this node.
			if (!queryresolver_handle_conditions(erow, qo)) {

#ifdef DEBUG_QUERYRESOLVER
				printf("queryresolver_handle_select_query(): The WHERE statement evaluated to false. Delete this node's entry.\n");
#endif // DEBUG_QUERYRESOLVER
				for (i = 0; i < QUERYRESOLVER_SENSORTABLE_SCHEME_COUNT; i++)
					erow[i] = 0;
			}

			// Write the new row in the sensortable.
#ifdef DEBUG_QUERYRESOLVER
			if (dbms_add_entry(stabid, queryresolver_sensortable_scheme,
							erow, QUERYRESOLVER_SENSORTABLE_SCHEME_COUNT) == 1)
			printf("queryresolver_handle_select_query(): WARNING: Couldn't insert entry in table %u!\n", stabid);
#else
			dbms_add_entry(stabid, queryresolver_sensortable_scheme, erow,
					QUERYRESOLVER_SENSORTABLE_SCHEME_COUNT);
#endif // DEBUG_QUERYRESOLVER
			// Create the projection's scheme which contains only selected cols.
			uint8_t ecount = queryresolver_get_requested_values_count(qo);
			//		printf(
			//				"queryresolver_handle_select_query(): number of requested values is %u\n",
			//				ecount);
			uint8_t projection_scheme[ecount];
			queryresolver_get_requested_values(qo, projection_scheme);

			// Create a projection to the sensortable to the rows that we need.
#ifdef DEBUG_QUERYRESOLVER
			if (dbms_select(stabid, projection_scheme, ecount) == 0)
			printf("queryresolver_handle_select_query(): WARNING: Select fail on table %u!\n", stabid);
#else
			dbms_select(stabid, projection_scheme, ecount);
#endif // DEBUG_QUERYRESOLVER

			// Make a dump from the temporary sensortable.

//			dbms_printf_table(stabid);

#ifdef DEBUG_QUERYRESOLVER
			if (resultdump == NULL)
			printf("queryresolver_handle_select_query(): WARNING: Couldn't get dump from table %u!\n", stabid);
#endif // DEBUG_QUERYRESOLVER
#ifdef DEBUG_QUERYMANAGER_TABLES
			printf("queryresolver_handle_select_query(): Send the following result table:\n");
			dbms_printf_table(stabid);
#endif // DEBUG_QUERYMANAGER_TABLES
			if (qo->temporary_table_id == 0) {
				//				printf(
				//						"queryresolver::queryresolver_handle_select_query() : no intermediate table id available\n");
				resultdump = dbms_get_dump(stabid, &size, NULL, NULL);

				//			qo->temporary_table_id = stabid;

			} else {
				//				printf(
				//						"queryresolver::queryresolver_handle_select_query() : intermediate table id available: %u\n",
				//						qo->temporary_table_id);
				qo->temporary_table_id = dbms_union(qo->temporary_table_id,
						stabid);

				// Check if any aggregation function is in the query and exe-
				// cute them, if exists.
				qo->temporary_table_id = querymanager_get_aggregation(qo,
						qo->temporary_table_id, 0);

//				dbms_printf_table(qo->temporary_table_id);

				resultdump = dbms_get_dump(qo->temporary_table_id, &size, NULL,
						NULL);

				//#ifdef DEBUG_QUERYRESOLVER
				//			if (dbms_drop_table(qo->temporary_table_id) != 0)
				//			printf("queryresolver_handle_select_query(): WARNING: Couldn't drop table %u!\n", qo->temporary_table_id);
				//#else
				//			dbms_drop_table(qo->temporary_table_id);
				//#endif // DEBUG_QUERYRESOLVER
				//			qo->temporary_table_id = 0;

			}

			// Remove the temporary tables.
#ifdef DEBUG_QUERYRESOLVER
			if (dbms_drop_table(stabid) != 0)
			printf("queryresolver_handle_select_query(): WARNING: Couldn't drop table %u!\n", stabid);
#else
			dbms_drop_table(stabid);
#endif // DEBUG_QUERYRESOLVER
		}
	} else {
		// generate result dump out of temporary data

		resultdump = dbms_get_dump(qo->temporary_table_id, &size, NULL, NULL);
	}

	// 2nd: do sending

	// Send the dump to the querymanager as result to this query.
	querymanager_send_result(resultdump, size, qo);

	// Free the memory dump
	free(resultdump);

}

/* --------------------------------------------------------------------
 * Read all from the query requested queries (also all SELECT state-
 * ments) and gets their values into the elements array.
 * @param int[]			Row of the temporary sensortable
 * @param query_object*	Pointer to the query object
 * @return				0 or non-zero, if an error occurs.
 * 1: Invalid field to select for.
 */
uint8_t queryresolver_read_requested_values(int erow[], query_object* qo) {

#ifdef DEBUG_QUERYRESOLVER
	printf("queryresolver_read_requested_values()\n");
#endif // DEBUG_QUERYRESOLVER
	// We need always the node's id, the query id and the current epoch.
	queryresolver_util_read_NODE_ID_TIKIDB(erow);
	queryresolver_util_read_query_id(erow, qo);
	queryresolver_util_read_epoch(erow, qo);

	// Get the first SELECT element.
	struct select_col *s = qo->the_query->select;

	int i;
//	int current_position = 3;

	// Iterate through all SELECT elements.
	for (i = 0; i < qo->the_query->num_selects; i++) {

		// Dispatch to the correct function to determine the
		//  requested sensors value.
		switch (s->value) {
		
		case NODE_ID_TIKIDB: {
			queryresolver_util_read_NODE_ID_TIKIDB(erow);
			break;
		}

		case CO2: {
			queryresolver_util_read_co2(erow);
			break;
		}

		case HUMIDITY: {
			queryresolver_util_read_humidity(erow);
			break;
		}

		case TEMPERATURE: {
			queryresolver_util_read_temperature(erow);
			break;
		}

		case RSSI: {
			queryresolver_util_read_rssi(erow);
			break;
		}

		default: {
#ifdef DEBUG_QUERYRESOLVER
			printf("queryresolver_read_requested_values(): WARNING: The type for SELECT doesn't exist. (%c %u)\n", s->value, s->value);
#endif // DEBUG_QUERYRESOLVER
			return 1;
		}
		}

//		current_position++;

		// Go to the next SELECT statement.
		s++;
	}

	return 0;
}

/* --------------------------------------------------------------------
 * Returns the sum of elements in the SELECT statement +3 for the node
 * id, epoch and qid.
 * @param query_object*	Pointer to the query object
 * @return				see above
 */
uint8_t queryresolver_get_requested_values_count(query_object* qo) {

#ifdef DEBUG_QUERYRESOLVER
	printf("queryresolver_get_requested_values_count()\n");
#endif // DEBUG_QUERYRESOLVER
	// Get the first SELECT element.
	struct select_col *s;// = qo->the_query->select;

	// Counter for the sum of elements in the SELECT statement.
	uint8_t ecount = 3;

	// Count the elements in the SELECT statement.
	int col_nr;
	for (col_nr = 0; col_nr < qo->the_query->num_selects; col_nr++) {
		s = &(qo->the_query->select[col_nr]);
		ecount++;
		if (s->aggregation != NONE)
			ecount++;
	}

	return ecount;
}

/* --------------------------------------------------------------------
 * Write all IDs from the SELECT statement in the array eids.
 * @param query_object*	Pointer to the query object
 * @param uint8_t*		Pointer to the array for the IDs.
 */
void queryresolver_get_requested_values(query_object* qo, uint8_t* eids) {

#ifdef DEBUG_QUERYRESOLVER
	printf("queryresolver_get_requested_values()\n");
#endif // DEBUG_QUERYRESOLVER
	// Get the first SELECT element.
	struct select_col *s = qo->the_query->select;

	// Counter for the sum of elements in the SELECT statement.
	uint8_t ecount = 3;

	// But first, write the IDs for the node id which we need always.
	eids[0] = NODE_ID_TIKIDB;
	eids[1] = QUERY_ID;
	eids[2] = EPOCH;

	// Go through the array for the second time and get now the
	//  ids.

	int i;
	for (i = 0; i < qo->the_query->num_selects; i++) {
		eids[ecount++] = s->value;
		s++;
	}

}

/* --------------------------------------------------------------------
 * Evaluate all conditions (WHERE) and returns their logical value.
 * @param int[]			Row of the temporary sensortable
 * @param query_object*	Pointer to the query object
 * @return				True (=1) or false (=0).
 */
uint8_t queryresolver_handle_conditions(int erow[], query_object* qo) {

#ifdef DEBUG_QUERYRESOLVER
	printf("queryresolver_handle_conditions()\n");
#endif // DEBUG_QUERYRESOLVER
	// Get the first WHERE element.
	struct condition *c = qo->the_query->conditions;

	// Initialize the loop: Set the "last" condition's value
	// to 1 (=true) and the conjunction to AND.
	uint8_t last_logical_value = 1;
	enum logic_conjunction last_logical_conjunction = AND;

	// Iterate through all WHERE elements.
	while (c) {

		// 1. Evaluate the actual condition.
		uint8_t actual_logical_value = queryresolver_evaluate_condition(
				erow[queryresolver_util_get_sensor_col(c->val1)], c->val2,
				c->op);

		// 2. Evaluate the last evaluated logical value with the
		// last conjunction and the value of the new condition and
		// save their logical value as new last value.
		queryresolver_evaluate_conjunction(last_logical_value,
				actual_logical_value, last_logical_conjunction);

		// 3. Save the current conjunction and evaluated value
		// for the next iteration.
		last_logical_conjunction = c->conjunction;
		last_logical_value = actual_logical_value;

		// Go to the next WHERE statement.
		c = c->next_condition;
	}

	// Return the last (also all) condition.
	return last_logical_value;
}

/* --------------------------------------------------------------------
 * Evaluate one condition.
 * @param enum attribute_type	The first operand contains an ID of a
 *  sensor or static value.
 * @param uint16_t				The second operand is a raw value
 * @param enum logic_operator	The logical operator of this condition
 * @return						0 or non-zero, if an error occurs.
 * 1: Operator is invalid.
 */
uint8_t queryresolver_evaluate_condition(enum attribute_type val1,
		uint16_t val2, enum comparison_operator op) {

#ifdef DEBUG_QUERYRESOLVER
	printf("queryresolver_evaluate_condition()\n");
#endif // DEBUG_QUERYRESOLVER
	// Evaluate with the correct operator.
	switch (op) {

	case SMALLER: {
		return (val1 < val2);
	}

	case SMALLER_EQUAL: {
		return (val1 <= val2);
	}

	case GREATER: {
		return (val1 > val2);
	}

	case GREATER_EQUAL: {
		return (val1 >= val2);
	}

	case EQUAL: {
		return (val1 == val2);
	}

	case NOT_EQUAL: {
		return !(val1 == val2);
	}

	default: {
#ifdef DEBUG_QUERYRESOLVER
		printf("queryresolver_evaluate_condition(): WARNING: The operator for this condition is invalid. Ignore it.\n");
#endif // DEBUG_QUERYRESOLVER
		return 1;
	}

	} // switch

}

/* --------------------------------------------------------------------
 * Evaluate one the conjunction of two conditions/logical values.
 * @param uint8_t				The first logical value
 * @param uint8_t				The second logical value
 * @param enum logic_operator	The conjunction beetween the values.
 * @return						True (=1) or false (=0).
 */
uint8_t queryresolver_evaluate_conjunction(uint8_t val1, uint8_t val2,
		enum logic_conjunction conj) {

#ifdef DEBUG_QUERYRESOLVER
	printf("queryresolver_evaluate_conjunction()\n");
#endif // DEBUG_QUERYRESOLVER
	switch (conj) {

	case AND: {
		return (val1 & val2);
	}

	case OR: {
		return (val1 | val2);
	}

	default: {
#ifdef DEBUG_QUERYRESOLVER
		printf("queryresolver_evaluate_conjunction(): WARNING: The operator %d for this condition is invalid. Will evaluate to false.\n", conj);
#endif // DEBUG_QUERYRESOLVER
		return 0;
	}

	} // switch

}

/* --------------------------------------------------------------------
 * Handle a query from the type LIST QUERIES.
 *
 * @param query_object*	Pointer to the query object
 */
void queryresolver_handle_list_queries_query(query_object *qo) {

#ifdef DEBUG_QUERYRESOLVER
	printf("queryresolver_handle_list_queries_query()\n");
#endif // DEBUG_QUERYRESOLVER
	// Create a new temporary table.
	uint8_t scheme[] = { NODE_ID_TIKIDB, QUERY_ID };
	uint8_t p_key[] = { NODE_ID_TIKIDB };
	uint8_t temporary_table_id;
	temporary_table_id = dbms_create_table(scheme, 2, p_key, 1);

#ifdef DEBUG_QUERYRESOLVER
	if (temporary_table_id <= 0)
	printf("queryresolver_handle_list_queries_query(): WARNING: Couldn't create temporary table! (%d)\n", temporary_table_id);
#endif // DEBUG_QUERYRESOLVER
	// Get the QIDs of the current running queries from this node
	// and save them in the temporary table.
	querymanager_get_active_periodic_queries(temporary_table_id);

#ifdef DEBUG_QUERYRESOLVER
	printf("queryresolver_handle_list_queries_query(): LIST QUERIES-Table from this node:\n");
	dbms_printf_table(temporary_table_id);
#endif // DEBUG_QUERYRESOLVER
	// Create a dump from the temporary table...
	unsigned int size;
	uint8_t* resultdump = dbms_get_dump(temporary_table_id, &size, NULL, NULL);

#ifdef DEBUG_QUERYRESOLVER
	if (resultdump == NULL)
	printf("queryresolver_handle_list_queries_query(): WARNING: Couldn't get dump from table %u!\n", temporary_table_id);
#endif // DEBUG_QUERYRESOLVER
	// ... and send it as the result to the querymanager.
	querymanager_send_result(resultdump, size, qo);

	// Free the memory dump
	free(resultdump);

	// Delete the temporary table.
#ifdef DEBUG_QUERYRESOLVER
	if (dbms_drop_table(temporary_table_id) != 0)
	printf("queryresolver_handle_list_queries_query(): WARNING: Couldn't drop table %u!\n", temporary_table_id);
#else
	dbms_drop_table(temporary_table_id);
#endif // DEBUG_QUERYRESOLVER
	// Remove this qo from the list.
	querymanager_qidlist_remove(qo);
	ctimer_stop(&qo->periodic_timer);
	ctimer_stop(&qo->in_slot_timer);

}

