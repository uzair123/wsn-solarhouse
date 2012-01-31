#include "stdlib.h"

#include "statement.h"
#include "lib/list.h"


static statement_id_t parse_statement_id(uint8_t *statement_arr);

static statement_t *unmarshall_open_scope(uint8_t* statement_arr,
		uint8_t statement_size);
static statement_t *unmarshall_close_scope(uint8_t* statement_arr,
		uint8_t statement_size);
static statement_t
*unmarshall_select(uint8_t* statement_arr, uint8_t statement_size);
static void parse_where_clause(select_statement_t* ss, uint8_t* statement_arr,
		uint8_t *index, uint8_t statement_size);
static void parse_group_by_clause(select_statement_t* ss,
		uint8_t* statement_arr, uint8_t *index, uint8_t statement_size);
static void parse_sample_period_clause(select_statement_t* ss,
		uint8_t* statement_arr, uint8_t *index, uint8_t statement_size);

/**
 * \brief		Parse the packet's data after the select statement.
 *
 * @param query* Pointer to the new query object.
 * @param uint8_t*  Pointer to the packet's data.
 * @param int    The actual index from which the data have to
 *               parse further.
 * @param int	 The length of the packet's data in byte.
 */
static statement_t *unmarshall_open_scope(uint8_t* statement_arr,
		uint8_t statement_size) {

	open_scope_statement_t *oss = (open_scope_statement_t*) malloc(
			sizeof(open_scope_statement_t));
	if (oss == NULL)
		return NULL;

	oss->type = OPEN_SCOPE_STATEMENT;
	oss->statement_id = parse_statement_id(statement_arr);

	oss->super_scope_id = statement_arr[3];
	oss->scope_id = statement_arr[4];
	oss->scope_ttl = ((statement_arr[5]) << 8) + statement_arr[6];
	oss->scope_spec_len = statement_arr[7];
	oss->scope_spec = malloc(oss->scope_spec_len);
	if (oss->scope_spec == NULL) {
		free(oss);
		return NULL;
	}
	memcpy(oss->scope_spec, statement_arr + 8, oss->scope_spec_len);

	return (statement_t*) oss;
}

static statement_t *unmarshall_close_scope(uint8_t* statement_arr,
		uint8_t statement_size) {

	close_scope_statement_t *css = (close_scope_statement_t*) malloc(
			sizeof(close_scope_statement_t));
	if (css == NULL)
		return NULL;

	css->type = CLOSE_SCOPE_STATEMENT;
	css->statement_id = parse_statement_id(statement_arr);

	css->scope_id = statement_arr[3];

	return (statement_t*) css;
}

/**
 * \brief		Parse the packet's data after the select statement.
 *
 * @param query* Pointer to the new query object.
 * @param uint8_t*  Pointer to the packet's data.
 * @param int    The actual index from which the data have to
 *               parse further.
 * @param int	 The length of the packet's data in byte.
 */
static statement_t *unmarshall_select(uint8_t* statement_arr, uint8_t statement_size) {
	uint8_t index = 4;
	int i;

	select_statement_t *ss = (select_statement_t*) malloc(
			sizeof(select_statement_t));
	if (ss == NULL)
		return NULL;

	ss->type = SELECT_STATEMENT;
	ss->statement_id = parse_statement_id(statement_arr);
	ss->disseminator = FALSE;

	LIST_STRUCT_INIT(ss, selects);
	LIST_STRUCT_INIT(ss, scopes);
	LIST_STRUCT_INIT(ss, conditions);

	// query_arr[index] contains the number n of the following arguments.
	uint8_t num_selects = statement_arr[index++];

//	PRINT_ARR(1,statement_arr, statement_size);
	//  Create structures for the select statement
	for (i = 0; i < num_selects; i++) {
		struct select_col_t *sc = (struct select_col_t*) malloc(
				sizeof(struct select_col_t));

		if (sc) {
			// statement_arr[index+n+1] contains the aggregation type
			sc->aggregation = statement_arr[index++];

			// statement_arr[index+n+2] contains the id of the col
			sc->value = statement_arr[index++];

			list_add(ss->selects, sc);
		} else
			return NULL;
	}

	uint8_t num_scopes = statement_arr[index++];
	//  Create structures for the select statement
	for (i = 0; i < num_scopes; i++) {

		struct from_scope_t *fs = (struct from_scope_t*) malloc(
				sizeof(struct from_scope_t));

		if (fs) {
			fs->scope_id = statement_arr[index++];

			list_add(ss->scopes, fs);
		} else
			return NULL;
	}

	// Go on with the next statement clauses, if exist.
	while (index+1 < statement_size) {

		// At least one further point exists.
		switch (statement_arr[index]) {

		case WHERE: { // WHERE
			parse_where_clause(ss, statement_arr, &index, statement_size);
			break;
		}

		case GROUP_BY: { // GROUP BY
			parse_group_by_clause(ss, statement_arr, &index, statement_size);
			break;
		}

		case SAMPLE_PERIOD: { // SAMPLE PERIOD
			parse_sample_period_clause(ss, statement_arr, &index,
					statement_size);
			break;
		}

		default: {
			return NULL;
		}
			break;

		} // switch

	}
	return (statement_t*) ss;
}

/**
 * \brief		Parse the packet's data after the select's argument statements.
 *
 * @param query* Pointer to the new query object.
 * @param uint8_t*  Pointer to the packet's data.
 * @param int    The actual index from which the data have to
 *               parse further.
 * @param int	 The length of the packet's data in byte.
 */
static void parse_where_clause(select_statement_t* ss, uint8_t* statement_arr,
		uint8_t *index, uint8_t statement_size) {

	int op_count = 0;
	int actual_index = (*index);
//	struct condition_t *old_cond = NULL;

	// At the beginning, it isn't known how many entries we have to parse.
	while (1) {

		// Allocate space for a new select structure.
		struct condition_t *c = (struct condition_t*) malloc(
				sizeof(struct condition_t));
		if (c == NULL) {
			return;
		}

//		// Link the last preceded condition with the new one.
//		if (op_count > 0) {
//			//			old_cond->next_condition = c;
//		} else {
//			ss->conditions = c;
//			//			c->next_condition = NULL;
//		}
//		old_cond = c;

		// Point to the operator of the first condition.
		c->op = statement_arr[actual_index + op_count * 4 + 1];

		// Point to the operator of the first operand.
		c->val1 = statement_arr[actual_index + op_count * 4 + 2];

		// Point to the operator of the second operand
		c->val2 = statement_arr[actual_index + op_count * 4 + 3];
		c->val2 = c->val2 << 8;
		c->val2 = c->val2 + statement_arr[actual_index + op_count * 4 + 4];

	} // while

}

/**
 * \brief		Parse the packet's data begining at the group by statement.
 *
 * @param query* Pointer to the new query object.
 * @param uint8_t*  Pointer to the packet's data.
 * @param int    The actual index from which the data have to
 *               parse further.
 * @param int	 The length of the packet's data in byte.
 */
static void parse_group_by_clause(select_statement_t* ss,
		uint8_t* statement_arr, uint8_t *index, uint8_t statement_size) {

	// Copy the ID of the GROUP BY col in the structure.
	ss->group_by = statement_arr[(*index) + 1];

	PRINTF(3,"index: %u, wq[index]:%u,%u,%u\n", *index, statement_arr[(*index)],
			statement_arr[(*index) + 1], statement_arr[(*index) + 2]);

}

/**
 * \brief		Parse the packet's data begining at the sample period statement.
 *
 * @param query* Pointer to the new query object.
 * @param uint8_t*  Pointer to the packet's data.
 * @param int    The actual index from which the data have to
 *               parse further.
 * @param int	 The length of the packet's data in byte.
 */
static void parse_sample_period_clause(select_statement_t* ss,
		uint8_t* statement_arr, uint8_t *index, uint8_t statement_size) {

	// Copy the time for the SAMPLE PERIOD in the structure.
	ss->sample_period = statement_arr[(*index) + 1];
	ss->sample_period <<= 8;
	ss->sample_period += statement_arr[(*index) + 2];
	ss->sample_period <<= 8;
	ss->sample_period += statement_arr[(*index) + 3];
	ss->sample_period <<= 8;
	ss->sample_period += statement_arr[(*index) + 4];

	*index = *index + 5;
	PRINTF(4,"[%u.%u:%10lu] %s::%s() : sample period %10lu, %u,%u,%u,%u\n",
			rimeaddr_node_addr.u8[1], rimeaddr_node_addr.u8[0], clock_time(), __FILE__, __FUNCTION__, ss->sample_period, statement_arr[(*index)+1], statement_arr[(*index)+2], statement_arr[(*index)+3], statement_arr[(*index)+4]);

	// Check whether exists an entry for LIFETIME
	if (((*index) + 1 < statement_size) && (statement_arr[(*index)]
			== LIFETIME)) {
		ss->lifetime = statement_arr[(*index) + 1];
		ss->lifetime <<= 8;
		ss->lifetime += statement_arr[(*index) + 2];
		ss->lifetime <<= 8;
		ss->lifetime += statement_arr[(*index) + 3];
		ss->lifetime <<= 8;
		ss->lifetime += statement_arr[(*index) + 4];

		*index = *index + 5;
		PRINTF(4,"[%u.%u:%10lu] %s::%s() : lifetime %10lu, %u,%u,%u,%u\n",
				rimeaddr_node_addr.u8[1], rimeaddr_node_addr.u8[0], clock_time(), __FILE__, __FUNCTION__, ss->lifetime, statement_arr[(*index)+6], statement_arr[(*index)+7], statement_arr[(*index)+8], statement_arr[(*index)+5]);
	}

	PRINTF(4, "finished parsing %p, has %u conds\n", ss, list_length(ss->conditions));
}

/**
 * \brief		TODO
 */
static statement_id_t parse_statement_id(uint8_t *statement_arr) {
	return statement_arr[1];
}


/**
 * \brief		TODO
 */
uint8_t *statement_array_get_height_ptr(uint8_t *statement_arr) {
	return (statement_arr + 3);
}

/* -------------------------------------------------------------------- */
/**
 * \brief		Takes a statement byte array and constructs the corresponding statement object
 *
 * \param statement_arr		pointer to the statement array.
 * \param statement_size	the length of the packet's data in byte.
 */
statement_t *statement_unmarshall(uint8_t* statement_arr,
		uint8_t statement_size) {

	statement_type type = statement_arr[2];
	switch (type) {
	case OPEN_SCOPE_STATEMENT: {
		return unmarshall_open_scope(statement_arr, statement_size);
	}
	case CLOSE_SCOPE_STATEMENT: {
		return unmarshall_close_scope(statement_arr, statement_size);
	}
	case SELECT_STATEMENT: {
		return unmarshall_select(statement_arr, statement_size);
	}
	case STOP_STATEMENT: {
		//TODO
		//statement->statement_id = statement_arr[index + 1];
		return NULL;
	}
	case LIST_QUERIES_STATEMENT: {
		// TODO
		return NULL;
	}
	default:
		return NULL;
	} // switch
}

