#ifndef TIKIDB_H
#define TIKIDB_H

#include <stdio.h>

#include "contiki.h"
#include "net/rime.h"

#include "database/dbms.h"
#include "querymanager/querymanager.h"
#include "queryresolver/queryresolver.h"

#endif // TIKIDB_H

// --------------------------------------------------------------------
// Callback variables
void (*callback_handle_result_function)(uint8_t* resultdump);
void (*callback_handle_error_function)(uint8_t* msg);


// --------------------------------------------------------------------
// Constructor and destructor
int tikidb_init(void (*callback_result)(uint8_t* resultdump, query_object *qo),
		void (*callback_error)(uint8_t* msg));
void tikidb_close(void);

// --------------------------------------------------------------------
// Interface methods
void tikidb_send_query(uint8_t* querystring, int size);


void tikidb_result_callback(uint8_t* resultdump);
void tikidb_error_callback(uint8_t* msg);


//int tikidb_set_location(int NODE_ID_TIKIDB_1, int NODE_ID_TIKIDB_2, int room, int floor,
//		int building, int x, int y, int z);
void tikidb_traffic_statistics_printf(void);
void tikidb_traffic_statistics_reset(void);
