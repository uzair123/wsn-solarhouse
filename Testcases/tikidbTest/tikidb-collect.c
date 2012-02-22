#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "dev/leds.h"
#include "node-id.h"
#include "sys/ctimer.h"
#include "tikidb.h"
#include "table.h"
#include "resultset.h"
#include "scopes-membership-simple.h"
#include "scopes-repository.h"
// sensor drivers according to tikidb wiki
#include "dev/light-sensor.h"
#include "dev/humidity-sensor.h"
#include "dev/tmp102.h"
#include "dev/sensorstation.h"


PROCESS(tikidb_collect_process, "tikidb collect");
AUTOSTART_PROCESSES(&tikidb_collect_process);

/*----------------------------------------------------------------------------*/
PROCESS_THREAD( tester_process, ev, data) {
	PROCESS_BEGIN();

	static struct etimer control_timer;
	leds_off(LEDS_ALL);

	/* wait a while */
	etimer_set(&control_timer, 10 * CLOCK_SECOND);
	PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&control_timer));
	PRINTF(1,"(TIKIDB TESTER) tester application started\n");

	// init tikidb and sensors
	tikidb_init(cb_shell_tikidb_result);
	light_sensor.configure(SENSORS_ACTIVE,1);
	humidity_sensor.configure(SENSORS_ACTIVE,1);
	tmp102_init();
	sensorstation_init();

	// sink node control
	if (node_id == 1) {
		static uint8_t scope_statement_arr[] = { //
				STATEMENT, // indicates that the byte array is a statement (instead of a result)
						11, // statement id
						OPEN_SCOPE_STATEMENT, // type of statement is OPEN SCOPE (instead e.g., of STOP or LIST_QUERIES)
						SCOPES_WORLD_SCOPE_ID, // if of super scope
						SCOPE_ID, // id of the scope
						0, // TTL
						40, //
						5, // scope spec length
						PREDICATE_GT, //
						REPOSITORY_VALUE, //
						NODE_ID, //
						UINT8_VALUE, //
						2 };

		PRINTF(1,"(TIKIDB TESTER) Opening scope %u\n", SCOPE_ID);
		tikidb_process_statement(scope_statement_arr,
				sizeof(scope_statement_arr));

		/* wait a while */
//		PRINTF(1,"(TIKIDB TESTER) Waiting 2.5 tree rebuild periods.\n");
//		etimer_set(&control_timer, 2.5 * SELFUR_ROUTING_REFRESH_INTERVAL* CLOCK_SECOND);
		PRINTF(1,"(TIKIDB TESTER) Waiting 1.1 tree rebuild periods\n");
				etimer_set(&control_timer, 1.1 * SELFUR_ROUTING_REFRESH_INTERVAL * CLOCK_SECOND);
//		etimer_set(&control_timer, 2.5 * CLOCK_SECOND);
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&control_timer));

		static uint8_t statement_arr[] = { //
				STATEMENT, /* indicates that the byte array is a statement (instead of a result) */
				21, /* statement id */
				SELECT_STATEMENT, /* type of statement is SELECT (instead of STOP or LIST_QUERIES) */
				0, /* placeholder in message for tree height */
				6, /* number of columns to project */
				NONE, AMBIENT_LIGHT, /* no aggregation, ambient light */
				NONE, PAR_LIGHT, /* no aggregation, light */
				NONE, HUMIDITY, /* no aggregation, humidity */
				NONE, TEMPERATURE, /* no aggregation, temp */
				NONE, NODE_RSSI, /* no aggregation, rssi */
				NONE, NODE_PARENT, /* no aggregation, node parent */
				1, /* number of scopes */
				SCOPE_ID, /* id of the scope */
				SAMPLE_PERIOD, /* next comes the sample period */
				SAMPLE_PERIOD_VALUE_1, /* */
				SAMPLE_PERIOD_VALUE_2, /* */
				SAMPLE_PERIOD_VALUE_3, /* */
				SAMPLE_PERIOD_VALUE_4, /* */
				LIFETIME, /* next comes the lifetime */
				LIFETIME_VALUE_1, /* */
				LIFETIME_VALUE_2, /* */
				LIFETIME_VALUE_3, /* */
				LIFETIME_VALUE_4 }; /* */

		tikidb_process_statement(statement_arr, sizeof(statement_arr));

		PRINTF(1,"(TIKIDB TESTER) Statement sent, using height: %u\n", scopes_selfur_get_max_tree_height());

		keep_running = TRUE;
		while (keep_running) {
			/* wait */
			etimer_set(&control_timer, CLOCK_SECOND * 10);
			PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&control_timer));
			PRINTF(1,"(TIKIDB TESTER) Waiting 10 seconds\n");
		}

		static uint8_t close_scope_statement_arr[] = { //
				STATEMENT, // indicates that the byte array is a statement (instead of a result)
						12, // statement id
						CLOSE_SCOPE_STATEMENT, // type of statement is OPEN SCOPE (instead e.g., of STOP or LIST_QUERIES)
						SCOPE_ID }; // id of the scope

		PRINTF(1,"(TIKIDB TESTER) Closing scope %u.\n", SCOPE_ID);
		tikidb_process_statement(close_scope_statement_arr,
				sizeof(close_scope_statement_arr));

		PRINTF(1,"(TIKIDB TESTER) Waiting 100 seconds.\n");
		etimer_set(&control_timer, 100 * CLOCK_SECOND);
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&control_timer));

	} // end of sink node control
	PROCESS_END();
}
