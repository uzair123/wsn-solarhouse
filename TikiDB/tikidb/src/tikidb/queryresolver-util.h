#ifndef QUERYRESOLVER_UTIL_H
#define QUERYRESOLVER_UTIL_H

#include "contiki.h"
#include "net/rime.h"

#include "dev/sht11-sensor.h"
#include "dev/light-sensor.h"
//#include "dev/acc-sensor.h"
//#include "dev/battery-sensor.h"

#include "queryresolver.h"
#include "resultset.h"
//#include "settings.h"
#include "scopes-selfur.h"



// --------------------------------------------------------------------
// Util methods
void queryresolver_util_read_NODE_ID_ATTR(int erow[]);
void queryresolver_util_read_epoch(int erow[], select_statement_t* ss);
void queryresolver_util_read_statement_id(int erow[], select_statement_t* ss);
void queryresolver_util_read_temperature(int erow[]);
void queryresolver_util_read_humidity(int erow[]);
void queryresolver_util_read_ambient_light(int erow[]);
void queryresolver_util_read_par_light(int erow[]);
void queryresolver_util_read_internal_temperature(int erow[]);
void queryresolver_util_read_internal_voltage(int erow[]);
void queryresolver_util_read_node_rssi(int erow[]);
void queryresolver_util_read_parent_node(int erow[], scope_id_t scope_id);
void queryresolver_util_read_user_defined(int erow[], uint8_t n);
void queryresolver_util_read_coordinate(int erow[], uint8_t n);
void queryresolver_util_read_acceleration(int erow[], uint8_t n);
void queryresolver_util_read_aggreation(int erow[], aggregation_type type);

// --------------------------------------------------------------------
// Helper methods
uint8_t queryresolver_util_get_sensor_col(attribute_type attrtype);

// --------------------------------------------------------------------
// Definitions for the sensor table
extern uint8_t queryresolver_sensortable_p_key[];
extern uint8_t queryresolver_sensortable_scheme[];
#define QUERYRESOLVER_SENSORTABLE_P_KEY_COUNT 3
#define QUERYRESOLVER_SENSORTABLE_SCHEME_COUNT 22+5



#endif // QUERYRESOLVER_UTIL_H
