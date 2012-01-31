#ifndef QUERYRESOLVER_UTIL_H
#define QUERYRESOLVER_UTIL_H
#include <stdio.h>
#include <stdlib.h>
#include "contiki.h"
#include "net/rime.h"

#include "lib/sensors.h"
#include "dev/cc2420.h"
#include "dev/sensorstation.h"		// MOD[David]: new sensor type
#include "dev/humidity-sensor.h"	// MOD[David]: humidity header-file
#include "dev/tmp102.h"				// MOD[David]: temperature header-file


#include "queryresolver.h"
#include "../debug.h"
#include "../tikidb-conf.h"
#include "../database/dbms.h"



// --------------------------------------------------------------------
// Util methods
void queryresolver_util_read_NODE_ID_TIKIDB(int erow[]);
void queryresolver_util_read_epoch(int erow[], query_object* qo);
void queryresolver_util_read_query_id(int erow[], query_object* qo);
void queryresolver_util_read_temperature(int erow[]);
void queryresolver_util_read_humidity(int erow[]);
void queryresolver_util_read_rssi(int erow[]);			// MOD[David]: Changed to single light value
void queryresolver_util_read_co2(int erow[]);			// MOD[David]: Second light value changed to CO2

// --------------------------------------------------------------------
// Helper methods
uint8_t queryresolver_util_get_sensor_col(enum attribute_type attrtype);


#endif // QUERYRESOLVER_UTIL_H
