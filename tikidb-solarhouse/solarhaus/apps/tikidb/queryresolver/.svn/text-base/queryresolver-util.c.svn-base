#include "node-id.h"
#include "queryresolver-util.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *  U T I L   M E T H O D S
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* --------------------------------------------------------------------
 * Read the node id from this node and write it into the sensortable.
 * @param uint8_t	ID of the temporary sensortable.
 */
void queryresolver_util_read_NODE_ID_TIKIDB(int erow[]) {
#ifdef DEBUG_QUERYRESOLVER
	printf("queryresolver_util_read_NODE_ID_TIKIDB()\n");
#endif // DEBUG_QUERYRESOLVER
	uint8_t col = queryresolver_util_get_sensor_col(NODE_ID_TIKIDB);
	erow[col] = node_id;
}


void queryresolver_util_read_query_id(int erow[], query_object* qo) {
#ifdef DEBUG_QUERYRESOLVER
	printf("queryresolver_util_read_query_id()\n");
#endif // DEBUG_QUERYRESOLVER
	uint8_t col = queryresolver_util_get_sensor_col(QUERY_ID);
	erow[col] = qo->qid;

}


void queryresolver_util_read_epoch(int erow[], query_object* qo) {
#ifdef DEBUG_QUERYRESOLVER
	printf("queryresolver_util_read_epoch()\n");
#endif // DEBUG_QUERYRESOLVER
	uint8_t col = queryresolver_util_get_sensor_col(EPOCH);
	erow[col] = qo->actual_lifetime / qo->the_query->sample_period;
}


void queryresolver_util_read_co2(int erow[]) {
#ifdef DEBUG_QUERYRESOLVER
	printf("queryresolver_util_read_co2()\n");
#endif // DEBUG_QUERYRESOLVER
	// Search the col for the data for the requested value.
	uint8_t col = queryresolver_util_get_sensor_col(CO2);
	if(node_id == 54 || node_id == 55){
		struct sensor_values _sensor_values;
		_sensor_values = sensorstation_read();
		erow[col] = _sensor_values.co2;			// MOD[David]: new temperature method
	}
	else{	
		erow[col] = 0;
	}
}


void queryresolver_util_read_temperature(int erow[]) {
#ifdef DEBUG_QUERYRESOLVER
	printf("queryresolver_util_read_temperature()\n");
	printf("tmp102_read_temp_raw() %d\n",tmp102_read_temp_raw());
#endif // DEBUG_QUERYRESOLVER
	uint8_t col = queryresolver_util_get_sensor_col(TEMPERATURE);
	if(node_id == 54 || node_id == 55){
		struct sensor_values _sensor_values;
		_sensor_values = sensorstation_read();
		erow[col] = _sensor_values.temperature;			// MOD[David]: new temperature method
	}
	else{	
		erow[col] = tmp102_read_temp_raw();
	}
}


void queryresolver_util_read_humidity(int erow[]) {
#ifdef DEBUG_QUERYRESOLVER
	printf("queryresolver_util_read_humidity()\n");
#endif // DEBUG_QUERYRESOLVER
	uint8_t col = queryresolver_util_get_sensor_col(HUMIDITY);
	if(node_id == 54 || node_id == 55){
		struct sensor_values _sensor_values;
		_sensor_values = sensorstation_read();
		erow[col] = _sensor_values.humidity;			// MOD[David]: new temperature method
	}
	else{	
		erow[col] = humidity_sensor.value(0);			// MOD[David]: new humidity method
	}
}


void queryresolver_util_read_rssi(int erow[]) {
#ifdef DEBUG_QUERYRESOLVER
	printf("queryresolver_util_read_rssi()\n");
#endif // DEBUG_QUERYRESOLVER
	uint8_t col = queryresolver_util_get_sensor_col(RSSI);
	erow[col] = cc2420_last_rssi;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *  H E L P E R   M E T H O D S
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* --------------------------------------------------------------------
 * Returns the number of column for the values for the current sensor.
 * @param enum attribute_type	Type-ID of the column.
 */
uint8_t queryresolver_util_get_sensor_col(enum attribute_type attrtype) {

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

