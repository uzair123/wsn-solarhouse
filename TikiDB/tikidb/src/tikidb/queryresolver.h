#ifndef QUERYRESOLVER_H
#define QUERYRESOLVER_H

#include "contiki.h"

#include "lib/sensors.h"
#include "dev/sht11.h"
#include "dev/light.h"
//#include "dev/acc-sensor.h"
#include "dev/battery-sensor.h"
#include "tikidb-types.h"
#include "queryresolver-util.h"
#include "resultset.h"

#include "scopes.h"


// --------------------------------------------------------------------
// Constructor and destructor
void queryresolver_init();
void queryresolver_close();

// --------------------------------------------------------------------
// Helper methods
void queryresolver_read_requested_values(int erow[],
		select_statement_t* ss);
void queryresolver_get_requested_values(select_statement_t* ss, uint8_t* eids);
bool queryresolver_handle_conditions(int erow[],
		select_statement_t* ss);





#endif // QUERYRESOLVER_H
