#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "contiki.h"
#include "dev/leds.h"
//#include "dev/button-sensor.h"
//#include "lib/random.h"
#include "node-id.h"
#include "sys/ctimer.h"

#include "tikidb.h"

#include "table.h"
#include "resultset.h"

#include "scopes-membership-simple.h"
#include "scopes-repository.h"

#define SCOPE_ID 5

// sample period is 1-minute==60 seconds
#define SAMPLE_PERIOD_VALUE_L (CLOCK_SECOND*60)
#define SAMPLE_PERIOD_VALUE_1 (SAMPLE_PERIOD_VALUE_L/(256UL*256*256))%256
#define SAMPLE_PERIOD_VALUE_2 (SAMPLE_PERIOD_VALUE_L/(256UL*256))%256
#define SAMPLE_PERIOD_VALUE_3 (SAMPLE_PERIOD_VALUE_L/256)%256
#define SAMPLE_PERIOD_VALUE_4 (SAMPLE_PERIOD_VALUE_L)%256

// lifetime is 5*256+160 samples == 1440 samples == 1440 * 60seconds == 1440* 1 minutes == 24*60 minutes == 24hours
#define LIFETIME_VALUE_L ((CLOCK_SECOND*60)*60*24)				//        11,059,200
#define LIFETIME_VALUE_1 (LIFETIME_VALUE_L/(256UL*256*256))%256	//   0 ==          0
#define LIFETIME_VALUE_2 (LIFETIME_VALUE_L/(256*256UL))%256		// 168 == 11,010,048
#define LIFETIME_VALUE_3 (LIFETIME_VALUE_L/256)%256				// 192 ==     49,152
#define LIFETIME_VALUE_4 (LIFETIME_VALUE_L)%256					//   0 ==          0
PROCESS(tester_process, "tikidb tester");
AUTOSTART_PROCESSES(&tester_process);

static bool keep_running;

/* helper functions */

void cb_shell_tikidb_result(uint8_t* result_dump, select_statement_t *ss) {

	PRINTF(
			4,
			"[%u.%u:%10lu] %s::%s() : function entered, result_dump: %p, query_object: %p, temp_table_id: %u\n",
			rimeaddr_node_addr.u8[1], rimeaddr_node_addr.u8[0], clock_time(),
			__FILE__, __FUNCTION__, result_dump, ss, ss->temporary_table_id);

	table_t *table = tikidb_buffer_get_table_id(ss->temporary_table_id);
	if (table) {
		uint8_t epoch = resultset_get_epoch(result_dump);
		uint8_t num_entries = 0;

		if (ss->group_by == 0) {
			num_entries = table->entries;
		} else {

			int i;

			for (i = 0; i < table->scheme_size; i++) {
				uint8_t col = table->scheme[i];
				if ((col == NONE) || (col == MAX) || (col == MIN) || (col
						== AVG) || (col == COUNT) || (col == SUM))
					break;
			}

			printf("HAD aggregation in column %u\n", i);

			if (i < table->scheme_size)
				num_entries = table_get_entry(table, 0)[i];
		}

		PRINTF(2, "[%u.%u:%10lu] %s::%s() : epoch {%u} results [%u]\n",
				rimeaddr_node_addr.u8[1], rimeaddr_node_addr.u8[0],
				clock_time(), __FILE__, __FUNCTION__, epoch, num_entries);
		table_printf(tikidb_buffer_get_table_id(ss->temporary_table_id));
	} else
		PRINTF(
				1,
				"[%u.%u:%10lu] %s::%s() : no table found with temp_table_id\n",
				rimeaddr_node_addr.u8[1], rimeaddr_node_addr.u8[0],
				clock_time(), __FILE__, __FUNCTION__);

}

PROCESS_THREAD( tester_process, ev, data) {
	PROCESS_BEGIN();

	static struct etimer control_timer;
	leds_off(LEDS_ALL);

	/* wait a while */
	etimer_set(&control_timer, 10 * CLOCK_SECOND);
	PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&control_timer));
	PRINTF(1,"(TIKIDB TESTER) tester application started\n");

	tikidb_init(cb_shell_tikidb_result);

	PRINTF(1,"(TIKIDB TESTER) ready to handle commands\n");

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

	}
	PROCESS_END();
}
