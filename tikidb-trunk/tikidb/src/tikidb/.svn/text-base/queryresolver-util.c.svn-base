#include "queryresolver-util.h"

#include "dev/cc2420.h"

// Definitions for the sensor table
uint8_t queryresolver_sensortable_p_key[] = { NODE_ID_ATTR, STATEMENT_ID, EPOCH };
uint8_t queryresolver_sensortable_scheme[] = { NODE_ID_ATTR, STATEMENT_ID, EPOCH,
		AMBIENT_LIGHT, PAR_LIGHT, HUMIDITY, TEMPERATURE, INTERNAL_TEMPERATURE,
		ACCELERATION_X, ACCELERATION_Y, ACCELERATION_Z, INTERNAL_VOLTAGE,
		X_COORDINATE, Y_COORDINATE, Z_COORDINATE, NODE_RSSI, NODE_PARENT,
		USER_DEFINED_1, USER_DEFINED_2, USER_DEFINED_3, MAX, MIN, AVG, COUNT,
		SUM };

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *  U T I L   M E T H O D S
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
 * \brief		Read the node id from this node and write it into the sensortable.
 *
 * \param erow[]	ID of the temporary sensortable.
 */
void queryresolver_util_read_NODE_ID_ATTR(int erow[]) {

#ifdef DEBUG_QUERYRESOLVER
	printf("queryresolver_util_read_NODE_ID_ATTR()\n");
#endif // DEBUG_QUERYRESOLVER
	// Search the col for the data for the requested value.
	uint8_t col = queryresolver_util_get_sensor_col(NODE_ID_ATTR);

	// Write the node's id in the new row.
	erow[col] = (int) rimeaddr_node_addr.u8[0]
			+ ((int) rimeaddr_node_addr.u8[1] << 8);
}

/* --------------------------------------------------------------------
 * Read the actual query id and write it into the sensortable.
 * @param uint8_t	ID of the temporary sensortable.
 * @param query_object*	Pointer to the actual qo.
 */
void queryresolver_util_read_statement_id(int erow[], select_statement_t* ss) {

#ifdef DEBUG_QUERYRESOLVER
	printf("queryresolver_util_read_query_id()\n");
#endif // DEBUG_QUERYRESOLVER
	// Search the col for the data for the requested value.
	uint8_t col = queryresolver_util_get_sensor_col(STATEMENT_ID);

	// Write the actual value.
	erow[col] = ss->statement_id;

}

/* --------------------------------------------------------------------
 * Read the actual epoch and write it into the sensortable.
 * @param uint8_t	ID of the temporary sensortable.
 * @param query_object*	Pointer to the actual qo.
 */
void queryresolver_util_read_epoch(int erow[], select_statement_t* ss) {

#ifdef DEBUG_QUERYRESOLVER
	printf("queryresolver_util_read_epoch()\n");
#endif // DEBUG_QUERYRESOLVER
	uint8_t col = queryresolver_util_get_sensor_col(EPOCH);

	erow[col] = ss->elapsed_lifetime / ss->sample_period;

}

/* --------------------------------------------------------------------
 * Read the actual temperature from the node's sensor and write it
 *  into the sensortable.
 * @param uint8_t	ID of the temporary sensortable.
 */
void queryresolver_util_read_temperature(int erow[]) {

#ifdef DEBUG_QUERYRESOLVER
	printf("queryresolver_util_read_temperature()\n");
#endif // DEBUG_QUERYRESOLVER
	// Search the col for the data for the requested value.
	uint8_t col = queryresolver_util_get_sensor_col(TEMPERATURE);

	// Read the actual temprature as raw value.
	//	erow[col] = sht11_temp();
	erow[col] = sht11_sensor.value(SHT11_SENSOR_TEMP);
	/*switch(rimeaddr_node_addr.u8[0]) {
	 case 1: erow[col] = 12; break;
	 case 2: erow[col] = 16; break;
	 case 3: erow[col] = 18; break;
	 case 4: erow[col] = 22; break;
	 case 5: erow[col] = 25; break;
	 default: erow[col] = 42; break;
	 }*/

}

/* --------------------------------------------------------------------
 * Read the humidity from this sensors node and write it into
 *  the sensortable.
 * @param uint8_t	ID of the temporary sensortable.
 */
void queryresolver_util_read_humidity(int erow[]) {

#ifdef DEBUG_QUERYRESOLVER
	printf("queryresolver_util_read_humidity()\n");
#endif // DEBUG_QUERYRESOLVER
	// Search the col for the data for the requested value.
	uint8_t col = queryresolver_util_get_sensor_col(HUMIDITY);

	// Read the actual humidity as raw value.
	//	erow[col] = sht11_humidity();
	erow[col] = sht11_sensor.value(SHT11_SENSOR_HUMIDITY);
}

/* --------------------------------------------------------------------
 * Read the ambient light and write it into the sensortable.
 * @param uint8_t	ID of the temporary sensortable.
 */
void queryresolver_util_read_ambient_light(int erow[]) {

#ifdef DEBUG_QUERYRESOLVER
	printf("queryresolver_util_read_ambient_light()\n");
#endif // DEBUG_QUERYRESOLVER
	// Search the col for the data for the requested value.
	uint8_t col = queryresolver_util_get_sensor_col(AMBIENT_LIGHT);

	erow[col] = light_sensor.value(LIGHT_SENSOR_TOTAL_SOLAR);
}

/* --------------------------------------------------------------------
 * Read the par light and write it into the sensortable.
 * @param uint8_t	ID of the temporary sensortable.
 */
void queryresolver_util_read_par_light(int erow[]) {

#ifdef DEBUG_QUERYRESOLVER
	printf("queryresolver_util_read_par_light()\n");
#endif // DEBUG_QUERYRESOLVER
	// Search the col for the data for the requested value.
	uint8_t col = queryresolver_util_get_sensor_col(PAR_LIGHT);

	erow[col] = light_sensor.value(LIGHT_SENSOR_PHOTOSYNTHETIC);

	PRINTF(4,"[%u.%u:%10lu] %s::%s() : light value read: %d\n",
			rimeaddr_node_addr.u8[1], rimeaddr_node_addr.u8[0], clock_time(), __FILE__, __FUNCTION__, erow[col]);

}

/* --------------------------------------------------------------------
 * Read the internal temperature and write it into the sensor table.
 * @param uint8_t	ID of the temporary sensor table.
 */
void queryresolver_util_read_internal_temperature(int erow[]) {

#ifdef DEBUG_QUERYRESOLVER
	printf("queryresolver_util_read_internal_temperature()\n");
#endif // DEBUG_QUERYRESOLVER
	// Search the col for the data for the requested value.
	uint8_t col = queryresolver_util_get_sensor_col(INTERNAL_TEMPERATURE);

	// Read the internal temperature as raw value.
	//	SENSORS_ACTIVATE(acc_sensor);
	//	erow[col] = acc_sensor.value(3);
	//	SENSORS_DEACTIVATE(acc_sensor);

	// since the accelerometer is not implemented, we provide a null value
	erow[col] = 0;
}

/* --------------------------------------------------------------------
 * Read the internal temperature and write it into the sensortable.
 * @param uint8_t	ID of the temporary sensortable.
 */
void queryresolver_util_read_internal_voltage(int erow[]) {

#ifdef DEBUG_QUERYRESOLVER
	printf("queryresolver_util_read_internal_voltage()\n");
#endif // DEBUG_QUERYRESOLVER
	// Search the col for the data for the requested value.
	uint8_t col = queryresolver_util_get_sensor_col(INTERNAL_VOLTAGE);

	// Read the actual temprature as raw value.
	erow[col] = battery_sensor.value(0);
}

///* --------------------------------------------------------------------
// * Detect the room in which this node is and write it into the
// *  sensortable.
// * @param uint8_t	ID of the temporary sensortable.
// */
//void queryresolver_util_read_room(int erow[]) {
//
//#ifdef DEBUG_QUERYRESOLVER
//	printf("queryresolver_util_read_room()\n");
//#endif // DEBUG_QUERYRESOLVER
//	// Search the col for the data for the requested value.
//	uint8_t col = queryresolver_util_get_sensor_col(ROOM);
//
//	// Get the current floor in which this node is.
//	erow[col] = settings_get_room();
//}
//
///* --------------------------------------------------------------------
// * Detect the floor in which this node is and write it into the
// *  sensortable.
// * @param uint8_t	ID of the temporary sensortable.
// */
//void queryresolver_util_read_floor(int erow[]) {
//
//#ifdef DEBUG_QUERYRESOLVER
//	printf("queryresolver_util_read_room()\n");
//#endif // DEBUG_QUERYRESOLVER
//	// Search the col for the data for the requested value.
//	uint8_t col = queryresolver_util_get_sensor_col(FLOOR);
//
//	// Get the current floor in which this node is.
//	erow[col] = settings_get_floor();
//}
//
///* --------------------------------------------------------------------
// * Detect the building in which this node is and write it into the
// *  sensortable.
// * @param uint8_t	ID of the temporary sensortable.
// */
//void queryresolver_util_read_building(int erow[]) {
//
//#ifdef DEBUG_QUERYRESOLVER
//	printf("queryresolver_util_read_building()\n");
//#endif // DEBUG_QUERYRESOLVER
//	// Search the col for the data for the requested value.
//	uint8_t col = queryresolver_util_get_sensor_col(BUILDING);
//
//	// Get the current building in which this node is.
//	erow[col] = settings_get_building();
//}

/* --------------------------------------------------------------------
 * Write a user defined value into the sensortable.
 * @param uint8_t	ID of the temporary sensortable.
 */
void queryresolver_util_read_aggreation(int erow[], aggregation_type type) {

#ifdef DEBUG_QUERYRESOLVER
	printf("queryresolver_util_read_aggreation()\n");
#endif // DEBUG_QUERYRESOLVER
	erow[queryresolver_util_get_sensor_col(type)] = 1;

#ifdef DEBUG_QUERYRESOLVER
	default :
	printf("queryresolver_util_read_aggreation(): n is invalid!\n");
#endif // DEBUG_QUERYRESOLVER
	// switch
}

/* --------------------------------------------------------------------
 * Write the last rssi value into the sensor table.
 */
void queryresolver_util_read_node_rssi(int erow[]) {
#ifdef DEBUG_QUERYRESOLVER
	printf("queryresolver_util_read_node_rssi()\n");
#endif // DEBUG_QUERYRESOLVER
	// Search the col for the data for the requested value.
	uint8_t col = queryresolver_util_get_sensor_col(NODE_RSSI);

	// Get the rssi value.
	erow[col] = cc2420_rssi() + 53;

}

/* --------------------------------------------------------------------
 * Write the node's parent for the given scope.
 */
void queryresolver_util_read_parent_node(int erow[], scope_id_t scope_id) {
#ifdef DEBUG_QUERYRESOLVER
	printf("queryresolver_util_read_parent_node()\n");
#endif // DEBUG_QUERYRESOLVER
	// Search the col for the data for the requested value.
	uint8_t col = queryresolver_util_get_sensor_col(NODE_PARENT);

	// Get the current building in which this node is.
	rimeaddr_t parent = scopes_selfur_get_scope_next_hop(scope_id);
	erow[col] = parent.u8[0];
}

/* --------------------------------------------------------------------
 * Write a user defined value into the sensortable.
 * @param uint8_t	ID of the temporary sensortable.
 */
void queryresolver_util_read_user_defined(int erow[], uint8_t n) {

#ifdef DEBUG_QUERYRESOLVER
	printf("queryresolver_util_read_building()\n");
#endif // DEBUG_QUERYRESOLVER
	switch (n) {
	case 1: {
		erow[queryresolver_util_get_sensor_col(USER_DEFINED_1)] = 42;
		break;
	}
	case 2: {
		erow[queryresolver_util_get_sensor_col(USER_DEFINED_2)] = 142;
		break;
	}
	case 3: {
		erow[queryresolver_util_get_sensor_col(USER_DEFINED_3)] = 242;
		break;
	}
#ifdef DEBUG_QUERYRESOLVER
		default :
		printf("queryresolver_util_read_building(): n is invalid!\n");
#endif // DEBUG_QUERYRESOLVER
	} // switch
}

/* --------------------------------------------------------------------
 * Get a coordinate and write it into the sensortable.
 * @param uint8_t	ID of the temporary sensortable.
 */
void queryresolver_util_read_coordinate(int erow[], uint8_t n) {

#ifdef DEBUG_QUERYRESOLVER
	printf("queryresolver_util_read_coordinate()\n");
#endif // DEBUG_QUERYRESOLVER
	switch (n) {
	case 0: {
		erow[queryresolver_util_get_sensor_col(X_COORDINATE)] = 0;//settings_get_x();
		break;
	}
	case 1: {
		erow[queryresolver_util_get_sensor_col(Y_COORDINATE)] = 0;//settings_get_x();
		break;
	}
	case 2: {
		erow[queryresolver_util_get_sensor_col(Z_COORDINATE)] = 0;//settings_get_x();
		break;
	}
#ifdef DEBUG_QUERYRESOLVER
		default :
		printf("queryresolver_util_read_coordinate(): n is invalid!\n");
#endif // DEBUG_QUERYRESOLVER
	} // switch
}

/* --------------------------------------------------------------------
 * Get a acceleration and write it into the sensortable.
 * @param uint8_t	ID of the temporary sensortable.
 */
/*void queryresolver_util_read_acceleration(int erow[], uint8_t n) {

 #ifdef DEBUG_QUERYRESOLVER
 printf("queryresolver_util_read_acceleration()\n");
 #endif // DEBUG_QUERYRESOLVER

 switch(n) {
 case 0: {
 erow[queryresolver_util_get_sensor_col(ACCELERATION_X)] = acc_sensor.value(1);
 break;
 }
 case 1: {
 erow[queryresolver_util_get_sensor_col(ACCELERATION_Y)] = acc_sensor.value(2);
 break;
 }
 case 2: {
 erow[queryresolver_util_get_sensor_col(ACCELERATION_Z)] = acc_sensor.value(3);
 break;
 }
 #ifdef DEBUG_QUERYRESOLVER
 default :
 printf("queryresolver_util_read_acceleration(): n is invalid!\n");
 #endif // DEBUG_QUERYRESOLVER
 } // switch

 }*/
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *  H E L P E R   M E T H O D S
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* --------------------------------------------------------------------
 * Returns the number of column for the values for the current sensor.
 * @param attribute_type	Type-ID of the column.
 */
uint8_t queryresolver_util_get_sensor_col(attribute_type attrtype) {

#ifdef DEBUG_QUERYRESOLVER
	printf("queryresolver_util_get_sensor_col()\n");
#endif // DEBUG_QUERYRESOLVER
	int col;
	for (col = 0; col < QUERYRESOLVER_SENSORTABLE_SCHEME_COUNT; col++) {
		if (queryresolver_sensortable_scheme[col] == attrtype) {
			// Col i contains the data for this sensor.
			return col;
		}
	}

#ifdef DEBUG_QUERYRESOLVER
	printf("queryresolver_util_get_sensor_col(): WARNING: Sensor %u doesn't exist! Will override the column for the sensor's id!\n", attrtype);
#endif // DEBUG_QUERYRESOLVER
	return 0;
}

