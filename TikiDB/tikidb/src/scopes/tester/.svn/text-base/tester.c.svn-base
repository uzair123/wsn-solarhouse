#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "contiki.h"
#include "dev/leds.h"
#include "dev/button-sensor.h"
#include "lib/random.h"
#include "sys/ctimer.h"
#include "node-id.h"
#include "cfs/cfs.h"
#include "cfs/cfs-coffee.h"

#include "scopes-types.h"
#include "scopes.h"
#include "scopes-selfur.h"
#include "scopes-membership-simple.h"
#include "scopes-application.h"
#include "scopes-repository.h"

#define SCOPES_TESTER_SID 1
#define ROOT_NODE_ID 1

/* super scope */
#define SUPER_SCOPE_ID 111
#define SUPER_SCOPE_TTL 30
//#define SUPER_SCOPE_SPEC PREDICATE_LET, REPOSITORY_VALUE, NODE_ID, UINT8_VALUE, 5
#define SUPER_SCOPE_SPEC PREDICATE_GET, REPOSITORY_VALUE, NODE_ID, UINT8_VALUE, 9

/* sub scope */
#define SUB_SCOPE_ID 222
//#define SUB_SCOPE_TTL 30
//#define SUB_SCOPE_SPEC PREDICATE_GET, REPOSITORY_VALUE, NODE_ID, UINT8_VALUE, 4

PROCESS(tester_process, "scopes tester")
;
AUTOSTART_PROCESSES(&tester_process);

static scope_id_t reply_scope_id;

//static unsigned char leds_status;

//static struct subscriber scopes_tester;
static struct ctimer timer;
static char
		*to_members_data =
				"111111111 222222222 333333333 444444444 555555555 666666666 777777777 888888888 ";
static char
		*to_creator_data =
				"111111111 222222222 333333333 444444444 555555555 666666666 777777777 888888888 ";
//static char *test_data = "111111111 222222222 333333333 444444444 555555555 666666666 777777777 888888888 999999999 000000000 111111111 222222222 ";

/* helper functions */

//static int
//readline(int fd, char *buffer)
//{
//  int i;
//  for (i = 0; i < LOG_MSG_MAX; i++) {
//    if (cfs_read(fd, buffer + i, 1) > 0) {
//      if (buffer[i] == '\n') {
//        return ++i;
//      }
//    }
//    else {
//      return 0;
//    }
//  }
//  return LOG_MSG_MAX;
//}

//static unsigned long
//time_milliseconds(void)
//{
//  clock_time_t cclock = clock_time();
//  unsigned long mseconds = clock_seconds() * 1000 + cclock - (cclock / CLOCK_SECOND) * CLOCK_SECOND;
//
//  return mseconds;
//}

/* time broadcast functions */

/* scope functions */

void add_scope(scope_id_t scope_id) {
	//  PRINTF(1,"(SCOPES TESTER) added scope: %u\n", scope->scope_id);
}

void remove_scope(scope_id_t scope_id) {
	//  PRINTF(1,"(SCOPES TESTER) removed scope: %u\n", scope->scope_id);
}

void join_scope(scope_id_t scope_id) {
	//  PRINTF(1,"(SCOPES TESTER) joined scope: %u\n", scope->scope_id);
	if (scope_id == SUPER_SCOPE_ID) {
		leds_on(LEDS_GREEN);
	} else if (scope_id == SUB_SCOPE_ID) {
		leds_on(LEDS_BLUE);
	}
}

void leave_scope(scope_id_t scope_id) {
	//  PRINTF(1,"(SCOPES TESTER) left scope: %u\n", scope->scope_id);
	if (scope_id == SUPER_SCOPE_ID) {
		leds_off(LEDS_GREEN);
	} else if (scope_id == SUB_SCOPE_ID) {
		leds_off(LEDS_BLUE);
	}
	if (node_id != ROOT_NODE_ID) {
		leds_off(LEDS_RED);
		ctimer_stop(&timer);
	}
}

void send_reply(void *nothing) {
	PRINTF(1,"(SCOPES TESTER) sending reply via scope %u\n", reply_scope_id);
	leds_off(LEDS_RED);
	scopes_send(SCOPES_TESTER_SID, reply_scope_id, TRUE, to_creator_data,
			strlen(to_creator_data));
//	free(msg);
}

void recv_data(scope_id_t scope_id, void *data, data_len_t data_len,
		bool to_creator, const rimeaddr_t *source) {

	PRINTF(1,"(SCOPES TESTER) received data via scope %u: (%u) %s\n", scope_id, data_len, (char*)data);

	if (node_id == ROOT_NODE_ID) {
		leds_toggle(LEDS_RED);
	} else {
		//		uint8_t *msg = (uint8_t *) malloc(data_len);
		//		if (msg != NULL) {
		//			memcpy(msg, data, data_len);
		leds_on(LEDS_RED);
		reply_scope_id = scope_id;
		clock_time_t delay = 5 * CLOCK_SECOND + (random_rand() % (5
				* CLOCK_SECOND));
		ctimer_set(&timer, delay, send_reply, NULL);
		PRINTF(1,"(SCOPES TESTER) reply timer set to ~%lu seconds\n", delay/CLOCK_SECOND);
		//		}
		//		PRINTF(1,"(SCOPES TESTER) NO MEMORY FOR ALLOCATING MSG\n");
	}
}

/* end scope functions */

static struct scopes_application tester_callbacks = { add_scope, remove_scope,
		join_scope, leave_scope, recv_data };

PROCESS_THREAD(tester_process, ev, data) {
	PROCESS_BEGIN();

		leds_off(LEDS_ALL);

		PRINTF(1,"%s (node id: %u)\n", "tester application started", node_id);

		scopes_init(&scopes_selfur, &simple_membership);
		scopes_register(SCOPES_TESTER_SID, &tester_callbacks);

		//  static struct scope *sub_scope;
		static uint8_t super_spec[] = {SUPER_SCOPE_SPEC};
		//  static uint8_t sub_spec[] = { SUB_SCOPE_SPEC };

		leds_off(LEDS_ALL);

		static struct etimer control_timer;
		//    static struct etimer button_timer;

		PRINTF(1,"(SCOPES TESTER) ready to handle commands\n");

		//    /* save led status */
		//    leds_status = leds_get();

		/* wait for button push */
		etimer_set(&control_timer, CLOCK_SECOND * 12);
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&control_timer));

		while(node_id == ROOT_NODE_ID) {

			//      /* blink green led */
			//      etimer_set(&button_timer, CLOCK_SECOND / 2);
			//      leds_on(LEDS_GREEN);
			//      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&button_timer));
			//      leds_off(LEDS_GREEN);
			//
			//      /* restore led status */
			//      leds_on(leds_status);


			/* run test */

			/* open super scope */
			PRINTF(1,"(SCOPES TESTER) opening super scope\n");
			if (scopes_open(SCOPES_TESTER_SID, SCOPES_WORLD_SCOPE_ID, SUPER_SCOPE_ID,
							super_spec, sizeof(super_spec)/sizeof(uint8_t), SCOPES_FLAG_NONE, SUPER_SCOPE_TTL)) {
				PRINTF(1,"(SCOPES TESTER) opening went fine\n");
			}
			else {
				PRINTF(1,"(SCOPES TESTER) opening failed\n");
			}

			/* wait a while */
			etimer_set(&control_timer, 5 * CLOCK_SECOND);
			PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&control_timer));
			PRINTF(1,"(HEIGHT) %u\n", scopes_selfur_get_max_tree_height());

			/* wait a while */
			etimer_set(&control_timer, 30 * CLOCK_SECOND);
			PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&control_timer));
			PRINTF(1,"(HEIGHT) %u\n", scopes_selfur_get_max_tree_height());

			/* wait a while */
			etimer_set(&control_timer, 30 * CLOCK_SECOND);
			PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&control_timer));
			PRINTF(1,"(HEIGHT) %u\n", scopes_selfur_get_max_tree_height());

			/* wait a while */
			etimer_set(&control_timer, 30 * CLOCK_SECOND);
			PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&control_timer));
			PRINTF(1,"(HEIGHT) %u\n", scopes_selfur_get_max_tree_height());

			/* wait a while */
			etimer_set(&control_timer, 100 * CLOCK_SECOND);
			PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&control_timer));

			/* open sub scope */
			//        PRINTF(1,"(SCOPES TESTER) opening sub scope\n");
			//       sub_scope = scopes_open(&scopes_tester, super_scope, SUB_SCOPE_ID,
			//                               sub_spec, sizeof(sub_spec)/sizeof(uint8_t), SCOPES_FLAG_NONE, SUB_SCOPE_TTL);


			PRINTF(1,"(SCOPES TESTER) sending message to super scope\n");
			scopes_send(SCOPES_TESTER_SID, SUPER_SCOPE_ID, FALSE, to_members_data, strlen(to_members_data));

			/* PRINTF(1,"(SCOPES TESTER) sending message to sub scope\n"); */
			/* scopes_send(&scopes_tester, sub_scope, FALSE, test_data, strlen(test_data)); */

			/* wait a while */
			etimer_set(&control_timer, 30 * CLOCK_SECOND);
			PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&control_timer));

			/* close super scope */
			PRINTF(1,"(SCOPES TESTER) closing super scope\n");
			scopes_close(SCOPES_TESTER_SID, SUPER_SCOPE_ID);

			PRINTF(1,"(SCOPES TESTER) finished loop, waiting to restart\n\n\n\n\n");
			etimer_set(&control_timer, 50 * CLOCK_SECOND);
			PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&control_timer));
		}

		//	while (1) {
		//		/* blink green led */
		//		etimer_set(&button_timer, CLOCK_SECOND / 2);
		//		leds_on(LEDS_GREEN);
		//		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&button_timer));
		//		leds_off(LEDS_GREEN);
		//
		//	}

		PROCESS_END();
	}
