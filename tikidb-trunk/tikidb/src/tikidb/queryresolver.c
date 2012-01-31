#include "queryresolver.h"

static bool evaluate_condition(attribute_type val1,
		uint16_t val2, comparison_operator op);
static uint8_t evaluate_conjunction(uint8_t val1, uint8_t val2,
		logic_conjunction conj);

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
void queryresolver_init() {

	// Initialize the sensors (telos-specific)

	SENSORS_ACTIVATE(light_sensor);
	SENSORS_ACTIVATE(sht11_sensor);
	SENSORS_ACTIVATE(battery_sensor);

	PRINTF(4,"[%u.%u:%10lu] %s::%s() : sensors activated\n",
			rimeaddr_node_addr.u8[1], rimeaddr_node_addr.u8[0], clock_time(), __FILE__, __FUNCTION__);

}

/* --------------------------------------------------------------------
 * Deconstructor
 */
void queryresolver_close() {
	SENSORS_DEACTIVATE(light_sensor);
	SENSORS_DEACTIVATE(sht11_sensor);
	SENSORS_DEACTIVATE(battery_sensor);
}

/* -------------------------------------------------------------------- */
/* -------------------------------------------------------------------- */
/* -------------------------------------------------------------------- */
/* -------------------------------------------------------------------- */
/* -------------------------------------------------------------------- */
/* -------------------------------------------------------------------- */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *  H E L P E R   M E T H O D S
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* --------------------------------------------------------------------
 * Read all from the query requested queries (also all SELECT state-
 * ments) and gets their values into the elements array.
 * @param int[]			Row of the temporary sensortable
 * @param query_object*	Pointer to the query object
 * 1: Invalid field to select for.
 */
void queryresolver_read_requested_values(int erow[], select_statement_t* ss) {

#ifdef DEBUG_QUERYRESOLVER
	printf("queryresolver_read_requested_values()\n");
#endif // DEBUG_QUERYRESOLVER
	// We need always the node's id, the query id and the current epoch.
	queryresolver_util_read_NODE_ID_ATTR(erow);
	queryresolver_util_read_statement_id(erow, ss);
	queryresolver_util_read_epoch(erow, ss);

	struct select_col_t *s;
	// Iterate through all SELECT elements.
	for (s = list_head(ss->selects); s != NULL; s = s->next) {

		if (s->aggregation != NONE)
			queryresolver_util_read_aggreation(erow, s->aggregation);

		// Dispatch to the correct function to determine the
		//  requested sensors value.
		switch (s->value) {

		case NODE_ID_ATTR: {
			queryresolver_util_read_NODE_ID_ATTR(erow);
			break;
		}

		case AMBIENT_LIGHT: {
			queryresolver_util_read_ambient_light(erow);
			break;
		}

		case PAR_LIGHT: {
			queryresolver_util_read_par_light(erow);
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

		case INTERNAL_TEMPERATURE: {
			queryresolver_util_read_internal_temperature(erow);
			break;
		}

			/*case ACCELERATION_X : {
			 queryresolver_util_read_acceleration(erow, 0);
			 break;
			 }

			 case ACCELERATION_Y : {
			 queryresolver_util_read_acceleration(erow, 1);
			 break;
			 }

			 case ACCELERATION_Z : {
			 queryresolver_util_read_acceleration(erow, 2);
			 break;
			 }*/

		case INTERNAL_VOLTAGE: {
			queryresolver_util_read_internal_voltage(erow);
			break;
		}

		case X_COORDINATE: {
			queryresolver_util_read_coordinate(erow, 0);
			break;
		}

		case Y_COORDINATE: {
			queryresolver_util_read_coordinate(erow, 1);
			break;
		}

		case Z_COORDINATE: {
			queryresolver_util_read_coordinate(erow, 2);
			break;
		}

			//		case ROOM: {
			//			queryresolver_util_read_room(erow);
			//			break;
			//		}
			//
			//		case FLOOR: {
			//			queryresolver_util_read_floor(erow);
			//			break;
			//		}
			//
			//		case BUILDING: {
			//			queryresolver_util_read_building(erow);
			//			break;
			//		}

		case USER_DEFINED_1: {
			queryresolver_util_read_user_defined(erow, 1);
			break;
		}

		case USER_DEFINED_2: {
			queryresolver_util_read_user_defined(erow, 2);
			break;
		}

		case USER_DEFINED_3: {
			queryresolver_util_read_user_defined(erow, 3);
			break;
		}
		case NODE_RSSI: {
			queryresolver_util_read_node_rssi(erow);
			break;
		}
		case NODE_PARENT: {
			queryresolver_util_read_parent_node(erow, ss->scope_id);
			break;
		}

			//		case MAX:
			//		case MIN:
			//		case AVG:
			//		case COUNT:
			//		case SUM: {
			//			erow[current_position] = 1
			//			break;
			//		}


		default: {
#ifdef DEBUG_QUERYRESOLVER
			printf("queryresolver_read_requested_values(): WARNING: The type for SELECT doesn't exist. (%c %u)\n", s->value, s->value);
#endif // DEBUG_QUERYRESOLVER
			return;
		}
		}

	}

}

/**
 * \brief		Puts the attributes from the SELECT statement into the projection scheme array
 *
 * \param ss				select statement from which the attributes must be read
 * \param projection_scheme	pointer to the array for the IDs.
 */
void queryresolver_get_requested_values(select_statement_t* ss, uint8_t* projection_scheme) {

	// Counter for the sum of elements in the SELECT statement.
	uint8_t ecount = 2;

	// But first, write the IDs for the node id which we need always.
	projection_scheme[0] = NODE_ID_ATTR;
	projection_scheme[1] = EPOCH;

	// Go through the scheme and put now the attributes.
	struct select_col_t *s;
	for (s = list_head(ss->selects); s != NULL; s = s->next) {
		projection_scheme[ecount++] = s->value;
		if (s->aggregation != NONE) {
			projection_scheme[ecount++] = s->aggregation;
		}
	}

}

/* --------------------------------------------------------------------
 * Evaluate all conditions (WHERE) and returns their logical value.
 * @param int[]			Row of the temporary sensortable
 * @param query_object*	Pointer to the query object
 * @return				True (=1) or false (=0).
 */
bool queryresolver_handle_conditions(int erow[], select_statement_t* ss) {

#ifdef DEBUG_QUERYRESOLVER
	printf("queryresolver_handle_conditions()\n");
#endif // DEBUG_QUERYRESOLVER

	struct condition_t *c;// = ss->select_statement->conditions;

	// Initialize the loop: Set the "last" condition's value
	// to 1 (=true) and the conjunction to AND.
	bool last_logical_value = TRUE;
	logic_conjunction last_logical_conjunction = AND;

	// Iterate through all WHERE elements.
	for (c = list_head(ss->conditions); c != NULL; c = c->next) {

		// 1. Evaluate the actual condition.
		bool actual_logical_value = evaluate_condition(
				erow[queryresolver_util_get_sensor_col(c->val1)], c->val2,
				c->op);

		// 2. Evaluate the last evaluated logical value with the
		// last conjunction and the value of the new condition and
		// save their logical value as new last value.
		evaluate_conjunction(last_logical_value,
				actual_logical_value, last_logical_conjunction);

		// 3. Save the current conjunction and evaluated value
		// for the next iteration.
		last_logical_conjunction = c->conjunction;
		last_logical_value = actual_logical_value;

	}

	// Return the last (also all) condition.
	return last_logical_value;
}

/* --------------------------------------------------------------------
 * Evaluate one condition.
 * @param attribute_type	The first operand contains an ID of a
 *  sensor or static value.
 * @param uint16_t				The second operand is a raw value
 * @param logic_operator	The logical operator of this condition
 * @return						0 or non-zero, if an error occurs.
 * 1: Operator is invalid.
 */
static bool evaluate_condition(attribute_type val1, uint16_t val2,
		comparison_operator op) {

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
		return FALSE;
	}

	} // switch

}

/* --------------------------------------------------------------------
 * Evaluate one the conjunction of two conditions/logical values.
 * @param uint8_t				The first logical value
 * @param uint8_t				The second logical value
 * @param logic_operator	The conjunction beetween the values.
 * @return						True (=1) or false (=0).
 */
uint8_t evaluate_conjunction(uint8_t val1, uint8_t val2,
		logic_conjunction conj) {

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

