#include "tikidb-mgr.h"

#include "lib/list.h"
#include "dev/leds.h"

// Helper functions
static uint8_t count_needed_cells(select_statement_t* ss);

static void do_select_statement(void *qo);
static void get_active_select_statements(table_id_t table_id);
static void disseminate_statement(uint8_t *statement_arr,
		uint8_t statement_arr_size, statement_t *statement);
static void handle_resultset(uint8_t* resultset_arr,
		data_len_t resultset_arr_size, scope_id_t scope_id);
static void forward_resultset(uint8_t* resultset_arr,
		data_len_t resultset_arr_size, select_statement_t *ss);
static table_id_t run_aggregation(select_statement_t* ss, table_id_t tid,
		uint8_t aggregate_over_epoch);
static bool handle_open_scope_statement(statement_t *statement);
static void handle_close_scope_statement(statement_t *statement);
static void handle_select_statement(void *statement);
static void handle_list_statement(statement_t *statement);
static void stop_select_statement(select_statement_t* ss);
static void handle_stop_statement(statement_t *ss);
static void handle_statement(uint8_t* statement_arr, data_len_t statement_size,
		bool disseminate, scope_id_t scope_id);

static void free_statement(statement_t *statement);
static void remove_statement(statement_t *statement);
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *  S C O P E S    F R A M E W O R K   V A R I A B L E S
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//static struct subscriber scopes_tikidb;

//static scope_id_t member_of = 0;
static struct scopes_application tikiDB_callbacks = { cb_add_scope,
		cb_remove_scope, cb_join_scope, cb_leave_scope, cb_receive_data };

/*---------------------------------------------------------------------------*/
/** \brief		TODO */
struct statement_node {
	/** \brief TODO*/
	struct statement_node *next;
	/** \brief TODO*/
	statement_t *statement;
};

/** \brief		@todo */
LIST( tikidb_mgr_statements);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *  C O N S T R U C T O R   A N D   D E C O N S T R U C T O R
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/**
 * \brief		Is called from the layer above (i.e., the tikidb module) and initializes the querymanager.
 */
void tikidb_mgr_init(void(*callback_result)(uint8_t* resultset_dump,
		select_statement_t *ss)) {

	PRINTF(3,"[%u.%u:%10lu] %s::%s() : function entered with params %p\n",
			rimeaddr_node_addr.u8[1], rimeaddr_node_addr.u8[0], clock_time(), __FILE__, __FUNCTION__, callback_result);

	// Save the address from the callback method
	querymanager_cb_result = callback_result;

	/** Initialize instance management lists */
	list_init(tikidb_mgr_statements);

	//	querymanager_qidlist_init();

	tikidb_buffer_init();

	queryresolver_init();

	scopes_init(&scopes_selfur, &simple_membership);

	scopes_register(TIKIDB_SUBID, &tikiDB_callbacks);

}

/*---------------------------------------------------------------------------*/
/**
 * \brief		TODO
 *
 * 				TODO
 */
static struct statement_node *lookup_statement_id(statement_id_t statement_id) {
	struct statement_node *sn;
	for (sn = list_head(tikidb_mgr_statements); sn != NULL; sn = sn->next)
		if (sn->statement->statement_id == statement_id)
			return sn;

	return NULL;
}

/**
 * \brief		Closes the network connections and clear up.
 */
void tikidb_mgr_close() {

	// Free all statements
	struct statement_node *sn;
	while ((sn = list_head(tikidb_mgr_statements)) != NULL) {
		statement_t *statement = sn->statement;
		remove_statement(statement);
		free_statement(statement);
	}

	tikidb_buffer_close();

	queryresolver_close();

	// TODO: close opened scopes for which this node is root
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *  I N T E R F A C E   M E T H O D S
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/**
 * \brief		Invoked from higher layer (tikidb)
 */
void tikidb_mgr_statement(uint8_t* statement_arr, data_len_t statement_size) {

	handle_statement(statement_arr, statement_size, TRUE, 0);

}

/**
 * \brief		Sends statement to the scope's member nodes
 */
static void disseminate_statement(uint8_t *statement_arr,
		data_len_t statement_arr_size, statement_t *statement) {

	// Replace the placeholder for height with the real tree height
	uint8_t *height_pos = statement_array_get_height_ptr(statement_arr);
	*height_pos = scopes_selfur_get_max_tree_height();
	PRINTF(4, "[%u.%u:%10lu] %s::%s() : Query will use the height %u.\n",
			rimeaddr_node_addr.u8[1], rimeaddr_node_addr.u8[0], clock_time(),
			__FILE__, __FUNCTION__, *height_pos);

	//	PRINT_ARR(1, statement_arr, statement_arr_size);

	// if query has no scopes, send it through world
	if (!(statement->type == SELECT_STATEMENT) || (list_length(
			((select_statement_t*) statement)->scopes) == 0)) {
		// either the statement wasn't a SELECT, or it was one but it had no scopes
		// thus, send through world scope
		scopes_send(TIKIDB_SUBID, SCOPES_WORLD_SCOPE_ID, FALSE, statement_arr,
				statement_arr_size);
	} else {
		select_statement_t *ss = (select_statement_t*) statement;

		struct from_scope_t *fs;
		for (fs = list_head(ss->scopes); fs != NULL; fs = fs->next) {

			PRINTF(
					3,
					"[%u.%u:%10lu] %s::%s() : We are gonna try sending to members through scope %u\n",
					rimeaddr_node_addr.u8[1], rimeaddr_node_addr.u8[0],
					clock_time(), __FILE__, __FUNCTION__, fs->scope_id);

			scopes_send(TIKIDB_SUBID, fs->scope_id, FALSE, statement_arr,
					statement_arr_size);

		}

	}
}

/**
 * \brief		TODO
 */
static void handle_select_statement(void *statement) {
	select_statement_t* ss = (select_statement_t *) statement;

	// One more epoch elapsed, so increment the actual elapsed time of the query
	ss->elapsed_lifetime += ss->sample_period;

	// If the statement was executed enough times, stop the timer.
	// In this case, this execution will be the last one.
	if (ss->elapsed_lifetime >= ss->lifetime) {
		PRINTF(3,"[%u.%u:%10lu] %s::%s() : Elapsed time reached query's lifetime, destroying query..\n",
				rimeaddr_node_addr.u8[1], rimeaddr_node_addr.u8[0], clock_time(), __FILE__, __FUNCTION__);
		stop_select_statement(ss);
		remove_statement((statement_t*) statement);
		free_statement((statement_t*) statement);
	} else { // Else set the timers

		clock_time_t slot_length = ss->sample_period / (clock_time_t)(
				ss->tree_height + 1);

		//		PRINTF(1,"tree h: %u, sample per %lu, slot len: %lu\n", ss->tree_height, ss->sample_period, slot_length);
		clock_time_t epoch = ss->elapsed_lifetime / ss->sample_period;
		if (epoch == 1) {
			PRINTF(3,"[%u.%u:%10lu] %s::%s() : Setting periodic timer for query %u for the first time.\n",
					rimeaddr_node_addr.u8[1], rimeaddr_node_addr.u8[0], clock_time(), __FILE__, __FUNCTION__, ss->statement_id);
			ctimer_set(&ss->periodic_timer, ss->sample_period,
					&handle_select_statement, ss);
		} else {
			PRINTF(3,"[%u.%u:%10lu] %s::%s() : Resetting periodic timer for query %u for epoch %lu.\n",
					rimeaddr_node_addr.u8[1], rimeaddr_node_addr.u8[0], clock_time(), __FILE__, __FUNCTION__, ss->statement_id, epoch);
			ctimer_reset(&ss->periodic_timer);
		}

		clock_time_t delay = slot_length * (ss->tree_height
				- scopes_selfur_get_node_tree_height()) + random_rand()
				% (slot_length / 2);

		ctimer_set(&ss->in_slot_timer, delay, &do_select_statement, ss);

		PRINTF(3,"[%u.%u:%10lu] %s::%s() : posted more timers at %10lu and %10lu, elapsed lifetime: %10lu, lifetime: %10lu, slot length: %10lu\n",
				rimeaddr_node_addr.u8[1], rimeaddr_node_addr.u8[0], clock_time(), __FILE__, __FUNCTION__,
				timer_remaining(&ss->periodic_timer.etimer.timer), delay,
				ss->elapsed_lifetime, ss->lifetime, slot_length);
	}

}

/**
 * \brief		Stops a running select statement (doesn't free it up from memory)
 */
static void stop_select_statement(select_statement_t* ss) {

	PRINTF(4,"[%u.%u:%10lu] %s::%s() : function entered, ss = %p\n",
			rimeaddr_node_addr.u8[1], rimeaddr_node_addr.u8[0], clock_time(), __FILE__, __FUNCTION__, ss);

	ctimer_stop(&ss->periodic_timer);
	ctimer_stop(&ss->in_slot_timer);

	PRINTF(4,"[%u.%u:%10lu] %s::%s() : Stopped query %u\n",
			rimeaddr_node_addr.u8[1], rimeaddr_node_addr.u8[0], clock_time(), __FILE__, __FUNCTION__, ss->statement_id);
}

/**
 * \brief Handle a query of type SELECT
 */
static void do_select_statement(void *v_ss) {

	select_statement_t *ss = (select_statement_t*) v_ss;
	PRINTF(4,"[%u.%u:%10lu] %s::%s() : function entered, qo: %p, qo's scope_id: %u\n",
			rimeaddr_node_addr.u8[1], rimeaddr_node_addr.u8[0], clock_time(), __FILE__, __FUNCTION__, ss, ss->scope_id);

	// 1st: do sensing iff node is member of the scope
	uint8_t *resultset_arr = NULL;
	data_len_t resultset_arr_size;

	if (!scopes_member_of(ss->scope_id)) {
		// generate result dump out of temporary data, i.e., data received from child nodes
		resultset_arr = table_marshall(tikidb_buffer_get_table_id(
				ss->temporary_table_id), &resultset_arr_size);
	} else {
		// generate result dump out of temporary data, i.e., data received from child nodes, + own sensor data
		//		if (ss->select_statement != NULL) {

		// Create a new temporary sensor table
		table_id_t id_temporary_table;

		id_temporary_table = dbms_create_table(
				queryresolver_sensortable_scheme,
				QUERYRESOLVER_SENSORTABLE_SCHEME_COUNT,
				queryresolver_sensortable_p_key,
				QUERYRESOLVER_SENSORTABLE_P_KEY_COUNT);

		// Initialize a new row
		int i;
		int erow[QUERYRESOLVER_SENSORTABLE_SCHEME_COUNT];
		for (i = 0; i < QUERYRESOLVER_SENSORTABLE_SCHEME_COUNT; i++)
			erow[i] = 0;

		// Get the actual sensor values for each SELECT-Statement
		queryresolver_read_requested_values(erow, ss);

		// Check all conditions. If they evaluate to false, delete
		//  all values from this node.
		if (!queryresolver_handle_conditions(erow, ss)) {

			for (i = 0; i < QUERYRESOLVER_SENSORTABLE_SCHEME_COUNT; i++)
				erow[i] = 0;
		}

		// Write the new row in the sensor table.
		dbms_add_entry(id_temporary_table, queryresolver_sensortable_scheme,
				erow, QUERYRESOLVER_SENSORTABLE_SCHEME_COUNT);

		// Create the projection's scheme which contains only selected cols.
		uint8_t ecount = count_needed_cells(ss);
		uint8_t projection_scheme[ecount];
		queryresolver_get_requested_values(ss, projection_scheme);

		// Create a projection to the table to the rows that we need.
		dbms_select(id_temporary_table, projection_scheme, ecount);
		// Make a dump from the temporary sensortable.

		//			dbms_printf_table(stabid);

		if (ss->temporary_table_id == 0) {
			resultset_arr = table_marshall(tikidb_buffer_get_table_id(
					id_temporary_table), &resultset_arr_size);

		} else {
			table_t *temp_table = tikidb_buffer_get_table_id(
					ss->temporary_table_id);
			table_printf(temp_table);

			table_t *union_table = table_union(tikidb_buffer_get_table_id(
					ss->temporary_table_id), tikidb_buffer_get_table_id(
					id_temporary_table));
			ss->temporary_table_id = union_table->id;

			// Check if any aggregation function is in the query and execute them, if exists.
			ss->temporary_table_id = run_aggregation(ss,
					ss->temporary_table_id, 0);

			resultset_arr = table_marshall(tikidb_buffer_get_table_id(
					ss->temporary_table_id), &resultset_arr_size);

		}

		table_printf(tikidb_buffer_get_table_id(id_temporary_table));

		// Remove the temporary tables.
		dbms_drop_table(id_temporary_table);
		//		}
	}

	// 2nd: do sending

	// Forward the resultset to the respective target
	forward_resultset(resultset_arr, resultset_arr_size, ss);

	// Free the memory dump
	free(resultset_arr);

}

/**
 * \brief		Handle a query from the type STOP.
 *
 * \param qo Pointer to the query object
 */
static void handle_stop_statement(statement_t *st) {
	stop_statement_t *stop_s = (stop_statement_t*) st;

	// Get the statement object with the ID that should be stopped
	struct statement_node *sn = lookup_statement_id(stop_s->statement_to_stop);

	// Call the routine in the querymanager to stop the query there.
	if ((sn != NULL) && (sn->statement->type == SELECT_STATEMENT)) {
		statement_t *select_s = sn->statement;
		stop_select_statement((select_statement_t*) select_s);
		remove_statement(select_s);
		free_statement(select_s);
	} else {
		PRINTF(1,"not statement found to stop!\n");
	}

	remove_statement(st);
	free_statement(st);

}

/**
 * \brief		Handle a query from the type LIST QUERIES.
 *
 * \param qo	Pointer to the query object
 */
void handle_list_statement(statement_t *statement) {
	list_statement_t *ls = (list_statement_t *) statement;

	// Create a new temporary table.
	uint8_t scheme[] = { NODE_ID_ATTR, STATEMENT_ID };
	uint8_t p_key[] = { NODE_ID_ATTR };

	uint8_t id_temporary_table = dbms_create_table(scheme, 2, p_key, 1);

	// Get the QIDs of the current running queries from this node
	// and save them in the temporary table.
	get_active_select_statements(id_temporary_table);

	// Create a dump from the temporary table...
	data_len_t resultset_arr_size;
	uint8_t* resultset_arr = table_marshall(tikidb_buffer_get_table_id(
			id_temporary_table), &resultset_arr_size);

	// ... and send it as the result to the tikidb_mgr. TODO Fix for results of lists
	forward_resultset(resultset_arr, resultset_arr_size, ls);

	// Free the memory dump
	free(resultset_arr);

	// Delete the temporary table.
	dbms_drop_table(id_temporary_table);

	// Remove this list_statement from the list.
	remove_statement(statement);
	free_statement(statement);
}

/**
 * \brief		Sends a result message to the network, if this node is a forwarder,
 * 				or passes it to the upper layer, if the node is the root.
 *
 * \param resultset		the message
 * \param int The size of the message
 */
static void forward_resultset(uint8_t* resultset_arr,
		data_len_t resultset_arr_size, select_statement_t *ss) {

	PRINTF(4,"[%u.%u:%10lu] %s::%s() : function entered, ss: %p\n",
			rimeaddr_node_addr.u8[1], rimeaddr_node_addr.u8[0], clock_time(), __FILE__, __FUNCTION__, ss);

	resultset_arr[0] = RESULT;
	resultset_arr[1] = ss->statement_id; // statement id is only one byte
	clock_time_t *slot_length_ptr = (clock_time_t*) (resultset_arr + 2);

	if (ss->sample_period != 0)
		*slot_length_ptr = ss->sample_period / //
				(ss->tree_height + 1); // slot length
	else
		*slot_length_ptr = ss->slot_length;

	PRINTF(2,"put %lu, %lu, %u\n", *slot_length_ptr, ss->sample_period, ss->tree_height);

	/* Are we at the root node of any of the scopes of this statement? */
	if (ss->disseminator == TRUE) {
		/* Yes, thus send results to upper layer */
		querymanager_cb_result(resultset_arr, ss);
	} else {
		/* No, still at intermediate node, so forward through scope */
		PRINTF(1, "[%u.%u:%10lu] %s::%s() : Sending data through scope %u\n",
				rimeaddr_node_addr.u8[1], rimeaddr_node_addr.u8[0],
				clock_time(), __FILE__, __FUNCTION__, ss->scope_id);
		scopes_send(TIKIDB_SUBID, ss->scope_id, TRUE, resultset_arr,
				resultset_arr_size);
	}

	// Free the memory from the old object.
	if (ss->temporary_table_id != 0) {
		//		dbms_printf_table(qo->temporary_table_id);
		dbms_drop_table(ss->temporary_table_id);
		ss->temporary_table_id = 0;
	}

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *  H E L P E R   M E T H O D S
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
 * \brief		Get all active periodic queries and write their id in a new table,
 * 				which ID is commit as parameter.
 *
 * \param uint8_t ID from a table to write the active periodic
 * 				  queries into it. The table has to be the scheme
 * 				  {NODE_ID_TIKIDB, QUERY_ID_1, QUERY_ID_2}.
 */
static void get_active_select_statements(table_id_t table_id) {

	// The scheme for the return table.
	uint8_t scheme[] = { NODE_ID_ATTR, STATEMENT_ID };

	// Go through the list of statement objects.
	struct statement_node *sn;

	for (sn = list_head(tikidb_mgr_statements); sn != NULL; sn = sn->next) {

		if (sn->statement->type == SELECT_STATEMENT) {
			// Get the current query and add a new entry in the table.
			int new_row[] = { (int) rimeaddr_node_addr.u8[0]
					+ ((int) rimeaddr_node_addr.u8[1] << 8),
					sn->statement->statement_id };

			dbms_add_entry(table_id, scheme, new_row, 2);
		}
	} // while

}

/**
 * \brief		TODO
 *
 * 				TODO
 */
static void free_statement(statement_t *statement) {
	switch (statement->type) {
	case OPEN_SCOPE_STATEMENT: {
		open_scope_statement_t *ost = (open_scope_statement_t*) statement;
		free(ost->scope_spec);
		break;
	}
	case CLOSE_SCOPE_STATEMENT: {
		break;
	}
	case SELECT_STATEMENT: {
		select_statement_t *ss = (select_statement_t*) statement;
		// Free the allocated memory for the list with the SELECT statements.
		while (list_length(ss->selects) > 0) {
			struct select_col_t *sc = list_head(ss->selects);
			list_remove(ss->selects, sc);
			free(sc);
		}

		while (list_length(ss->scopes) > 0) {
			struct from_scope_t *sc = list_head(ss->scopes);
			list_remove(ss->scopes, sc);
			free(sc);
		}

		while (list_length(ss->conditions) > 0) {
			struct condition_t *co = list_head(ss->conditions);
			list_remove(ss->conditions, co);
			free(co);
		}
		break;
	}
	case STOP_STATEMENT:
	case LIST_QUERIES_STATEMENT: {
		break;
	} // case
	} // switch
	free(statement);
}

static void remove_statement(statement_t *statement) {
	struct statement_node *sn;
	if ((sn = lookup_statement_id(statement->statement_id)) != NULL)
		list_remove(tikidb_mgr_statements, sn);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *  S C O P E S    C A L L B A C K   F U N C T I O N S
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/**
 * \brief		TODO
 */
void cb_add_scope(scope_id_t scope_id) {
	//	PRINTF(1, "[%u.%u:%10lu] %s::%s() : Scope %u was added.\n",
	//			rimeaddr_node_addr.u8[1], rimeaddr_node_addr.u8[0], clock_time(),
	//			__FILE__, __FUNCTION__, scope_id);
	PRINTF(2, "(QUERYMANAGER) Node added scope %u\n" , scope_id);
}

/**
 * \brief		TODO
 */
void cb_remove_scope(scope_id_t scope_id) {
	//	PRINTF(3, "[%u.%u:%10lu] %s::%s() : Scope %u was removed.\n",
	//			rimeaddr_node_addr.u8[1], rimeaddr_node_addr.u8[0], clock_time(),
	//			__FILE__, __FUNCTION__, scope_id);
	PRINTF(2, "(QUERYMANAGER) Node removed scope %u\n" , scope_id);
}

/**
 * \brief		TODO
 */
void cb_join_scope(scope_id_t scope_id) {
	//	PRINTF(1, "[%u.%u:%10lu] %s::%s() : Node is member of scope %u\n",
	//			rimeaddr_node_addr.u8[1], rimeaddr_node_addr.u8[0], clock_time(),
	//			__FILE__, __FUNCTION__, scope_id);
	PRINTF(2, "(QUERYMANAGER) Node joined scope %u\n" , scope_id);
	leds_on(LEDS_BLUE);

	// TODO: request parent node all running queries for the newly joined scope
}

/**
 * \brief		TODO
 */
void cb_leave_scope(scope_id_t scope_id) {
	//	PRINTF(
	//			3,
	//			"[%u.%u:%10lu] %s::%s() : Node is not member of scope %u anymore, checking related queries...\n",
	//			rimeaddr_node_addr.u8[1], rimeaddr_node_addr.u8[0], clock_time(),
	//			__FILE__, __FUNCTION__, scope_id);
	PRINTF(2, "(QUERYMANAGER) Node left scope %u\n" , scope_id);
	leds_off(LEDS_BLUE);
	//	member_of = 0;


	// if there are queries running on that scope, stop them
	struct statement_node *sn;
	statement_t *s;
	for (sn = list_head(tikidb_mgr_statements); sn != NULL; sn = sn->next) {
		s = sn->statement;
		if ((s->type == SELECT_STATEMENT)
				&& (((select_statement_t*) s)->scope_id == scope_id)) {

			// stop query:
			stop_select_statement((select_statement_t*) s);
			remove_statement(s);
			free_statement(s);
		}
	}
}

/**
 * \brief		Invoked from Scopes framework when data is received
 */
void cb_receive_data(scope_id_t scope_id, void *data, data_len_t data_len,
		bool to_creator, const rimeaddr_t *source) {

	// Switch to the correct function depending on the packet type.
	switch (((uint8_t*) data)[0]) {
	case STATEMENT: {
		handle_statement(data, data_len, FALSE, scope_id);
		break;
	}
	case RESULT: {
		handle_resultset(data, data_len, scope_id);
		break;
	}
	default: {
		PRINTF(3, "[%u.%u:%10lu] %s::%s() : The packet is invalid and will be thrown away\n",
				rimeaddr_node_addr.u8[1], rimeaddr_node_addr.u8[0],
				clock_time(), __FILE__, __FUNCTION__);
		return;
	}
	}
}

/**
 * \brief		Handles a generic statement coming either from the bottom
 * 				layer (scopes) or from the higher layer (tikidb).
 */
static void handle_statement(uint8_t* statement_arr, data_len_t statement_size,
		bool disseminate, scope_id_t scope_id) {

	//	PRINTF(3,"[%u.%u:%10lu] %s::%s() : Received statement with id %u\n",
	//			rimeaddr_node_addr.u8[1], rimeaddr_node_addr.u8[0], clock_time(), __FILE__, __FUNCTION__,
	//			statement_id);

	/* Parse the statement */
	statement_t *statement =
			statement_unmarshall(statement_arr, statement_size);

	/* If it was successfully parsed and there is no statement with same id: */
	if ((statement != NULL) && (!lookup_statement_id(statement->statement_id))) {

		struct statement_node *sn = malloc(sizeof(struct statement_node));

		if (sn == NULL) {
			free_statement(statement);
			return;
		}

		sn->statement = statement;
		list_add(tikidb_mgr_statements, sn);

		// Determine the type of the statement and switch to the
		//  handler function for this type.
		switch (statement->type) {

		case OPEN_SCOPE_STATEMENT: {
			handle_open_scope_statement(statement);
			break;
		}

		case CLOSE_SCOPE_STATEMENT: {
			handle_close_scope_statement(statement);
			break;
		}

		case SELECT_STATEMENT: {
			select_statement_t *ss = (select_statement_t*) statement;

			if (disseminate) {
				ss->disseminator = TRUE;
				ss->tree_height = scopes_selfur_get_max_tree_height();
			} else {
				ss->scope_id = scope_id;
				ss->tree_height
						= *statement_array_get_height_ptr(statement_arr);
			}
			handle_select_statement(statement);

			if (disseminate)
				disseminate_statement(statement_arr, statement_size, statement);

			break;
		}

		case STOP_STATEMENT: {
			handle_stop_statement(statement);
			if (disseminate)
				disseminate_statement(statement_arr, statement_size, statement);
			break;
		}

		case LIST_QUERIES_STATEMENT: {
			handle_list_statement(statement);
			if (disseminate)
				disseminate_statement(statement_arr, statement_size, statement);
			break;
		}

		default: {
			PRINTF(3,"statement type is invalid; Abort\n");
		}

		} // switch
	}
	else {
		PRINTF(1, "Problems decoding statement or already exists a statement with id %u\n", statement_arr[1]);
		if (statement != NULL)
			free_statement(statement);
	}
}

/**
 * \brief		Handles a result dump coming from the network.
 */
static void handle_resultset(uint8_t* resultset_arr,
		data_len_t resultset_arr_size, scope_id_t scope_id) {

	uint16_t epoch;
	select_statement_t *ss = NULL;

	// search for an unused id
	table_id_t received_table_id = tikidb_buffer_get_free_id();

	// unmarshall resultset into table
	table_t *received_table = table_unmarshall(received_table_id,
			resultset_arr);

	if (received_table == NULL)
		return;

	/* We need to get a pointer to a statement object */

	/* Does a statement object exist for the specified statement id? */
	statement_id_t statement_id = received_table->statement_id;
	struct statement_node *sn = lookup_statement_id(statement_id);
	if (sn != NULL)
		ss = (select_statement_t*) (sn->statement);

//	PRINTF(1,"Got ResultSet, sn: %p statem: %p\n", sn, ss);
//
//	PRINT_ARR(1, resultset_arr, resultset_arr_size);
	/* Check to see whether this node also member of the scope, or it is simply a forwarder */
	if (scopes_member_of(scope_id)) {
		/* (this node is member of the scope through which the resultset was received) */
		if (ss != NULL)
			epoch = ss->elapsed_lifetime / ss->sample_period;
		else {
			/* this shouldn't happen! */
			PRINTF(1, "[%u.%u:%10lu] %s::%s() : There is no statement for the statement id received in the resultset (%u)!\n",
					rimeaddr_node_addr.u8[1], rimeaddr_node_addr.u8[0],
					clock_time(), __FILE__, __FUNCTION__, statement_id);
			//			epoch = 0;
			return;
		}

	} else if (scopes_selfur_is_forwarder(scope_id)) {
		/* (this node is not member of the scope through which the resultset was received, but
		 * it is a forwarder for that scope) */

		epoch = resultset_get_epoch(resultset_arr);

		if (ss == NULL) {

			ss = (select_statement_t *) malloc(sizeof(select_statement_t));
			if (!ss)
				return;

			// Initialize the (dummy) select_statement_t*:
			ss->type = SELECT_STATEMENT;
			ss->statement_id = statement_id;
			//			ss->tree_height = resultset_get_height_ptr(statement_arr)[0];
			ss->scope_id = scope_id;
			// Initialize the temporary_table_id
			ss->temporary_table_id = 0;
			// Initialize the execution counter.
			ss->elapsed_lifetime = 0;

			ss->sample_period = 0;
			ss->slot_length = *((clock_time_t*) (resultset_arr + 2));

			sn = (struct statement_node*) malloc(sizeof(struct statement_node));
			sn->statement = (statement_t*) ss;
			list_add(tikidb_mgr_statements, sn);

			PRINTF(3,"[%u.%u:%10lu] %s::%s() : No statement object for the received resultset, assuming forwarder (trigger in %lu)\n",
					rimeaddr_node_addr.u8[1], rimeaddr_node_addr.u8[0], clock_time(), __FILE__, __FUNCTION__, ss->slot_length);

			ctimer_set(&ss->in_slot_timer, ss->slot_length,
					&do_select_statement, ss);
		} else {
			ss = (select_statement_t*) sn->statement;
			//			PRINT_ARR(1, resultset, resultset_size);

			ctimer_reset(&ss->in_slot_timer);
		}
	} else {
		PRINTF(1,"[%u.%u:%10lu] %s::%s() : This node is neither member nor forwarder of scope id %u\n",
				rimeaddr_node_addr.u8[1], rimeaddr_node_addr.u8[0], clock_time(), __FILE__, __FUNCTION__, scope_id);
		return;
	}


	/* 2. Check if the epoch of the received resultset matches that of the local statement */
	if (epoch != resultset_get_epoch(resultset_arr)) {
		PRINTF(2,"[%u.%u:%10lu] %s::%s() : Discarding %u received results from epoch %u, this node's epoch is %u\n",
				rimeaddr_node_addr.u8[1], rimeaddr_node_addr.u8[0], clock_time(), __FILE__, __FUNCTION__,
				resultset_get_num_entries(resultset_arr), resultset_get_epoch(resultset_arr), epoch);
	} else {

		PRINTF(3,"ss: %p, epoch:%u, table %p, tableid %u\n", ss, epoch, received_table, received_table_id);
		// add table to buffer
		if ((received_table == NULL) || (!tikidb_buffer_add_table(
				received_table))) {
			PRINTF(3,"ERROR: Error while adding to list!\n\r");
			table_free(received_table);
			return;
		}

		// Merge the new table with the current one
		if (ss->temporary_table_id != 0) {

			// Execute the union
			table_t *union_table = table_union(tikidb_buffer_get_table_id(
					ss->temporary_table_id), received_table);
			ss->temporary_table_id = union_table->id;

			PRINTF(3,"[%u.%u:%10lu] %s::%s() : there already was a temporary table with id %u\n",
					rimeaddr_node_addr.u8[1], rimeaddr_node_addr.u8[0], clock_time(), __FILE__, __FUNCTION__,
					ss->temporary_table_id);

			// Remove the temporary table.
			dbms_drop_table(received_table_id);

		} else {
			ss->temporary_table_id = received_table_id;

			PRINTF(1,"[%u.%u:%10lu] %s::%s() : there was no temporary table, created one with id %u\n",
					rimeaddr_node_addr.u8[1], rimeaddr_node_addr.u8[0], clock_time(), __FILE__, __FUNCTION__, ss->temporary_table_id);

		}

	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *  S C O P E S    F R A M E W O R K   F U N C T I O N S
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
 * \brief		TODO
 */
static bool handle_open_scope_statement(statement_t *statement) {
	open_scope_statement_t *oss = (open_scope_statement_t*) statement;
	PRINTF(3,"[%u.%u:%10lu] %s::%s() : Opening scope.\n",
			rimeaddr_node_addr.u8[1], rimeaddr_node_addr.u8[0], clock_time(), __FILE__, __FUNCTION__);

	bool result = scopes_open(TIKIDB_SUBID, oss->super_scope_id, oss->scope_id,
			oss->scope_spec, oss->scope_spec_len, SCOPES_FLAG_NONE,
			oss->scope_ttl);

	remove_statement(statement);
	free_statement(statement);
	return result;

}

/**
 * \brief		TODO
 */
static void handle_close_scope_statement(statement_t *statement) {
	close_scope_statement_t *cst = (close_scope_statement_t*) statement;
	PRINTF(3,"[%u.%u:%10lu] %s::%s() : Closing scope\n",
			rimeaddr_node_addr.u8[1], rimeaddr_node_addr.u8[0], clock_time(), __FILE__, __FUNCTION__);

	scopes_close(TIKIDB_SUBID, cst->scope_id);
	remove_statement(statement);
	free_statement(statement);
}

/**
 * \brief		Returns the first (!) aggregation type from a query.
 *
 * \param query_object	The query object
 * \param table_id_t	ID of the temporary table with the results.
 * \param uint8_t		If 1, then it will be grouped over the epoch row;
 *                  	else over the row in s->value (also the structure).
 * \return table_id_t	The id of the old table
 */
static table_id_t run_aggregation(select_statement_t* ss, table_id_t tid,
		uint8_t aggregate_over_epoch) {

	if (ss->group_by != 0) {

		struct select_col_t *s;

		// Go through the SELECT statement.
		for (s = list_head(ss->selects); s != NULL; s = s->next) {

			PRINTF(3,"querymanager_get_aggregation(): col: %u, value %u\n",
					s->aggregation, s->value);

			if (s->aggregation != NONE) {

				// Perform the aggregation operation

				// Save the scheme of the original table for the projection.
				unsigned int tid_scheme_size = 0;
				uint8_t *original_table_scheme = dbms_get_table_scheme(tid,
						&tid_scheme_size);
				uint8_t *tid_scheme = (uint8_t*) malloc(tid_scheme_size
						* sizeof(uint8_t));
				memcpy(tid_scheme, original_table_scheme, tid_scheme_size);

				// Found an aggregation. Execute this aggregation on the new union table.
				// TODO Extend this to handle more operators.
				uint8_t rrr[1];
				rrr[0] = ss->group_by;

				if (aggregate_over_epoch == 1) {
					rrr[0] = EPOCH;
				}

				PRINTF(3,"querymanager_get_aggregation(): group[0] %u\n", rrr[0]);
				table_id_t new_tid = dbms_aggregate(tid, s->aggregation,
						s->value, // AGGR
						rrr, 1); // GROUP BY


				if (new_tid == 0) {
					PRINTF(3,"querymanager_get_aggregation(): new_tid was 0!!\n");
					return tid;
				}
				//				else
				//					dbms_printf_table(new_tid);

				return new_tid;

			}
			s++;
		}
	}

	// No aggregation was necessary. Give the "old" table id back.
	return tid;
}

/**
 * \brief		Calculates the amount of cells that are need for a given SELECT statement.
 * 				This is obtained by adding 2 (for the node id and epoch) to
 * 				the amount of columns on which it will be projected, plus 1 cell for each
 * 				attribute on which aggregation must be performed
 *
 * \param ss	select statement
 * \return		see above
 */
static uint8_t count_needed_cells(select_statement_t* ss) {

	struct select_col_t *s;

	// Counter for the sum of elements in the SELECT statement.
	uint8_t ecount = 2;

	// Count the elements in the SELECT statement.
	for (s = list_head(ss->selects); s != NULL; s = s->next) {
		ecount++;
		if (s->aggregation != NONE)
			ecount++;
	}

	return ecount;
}

