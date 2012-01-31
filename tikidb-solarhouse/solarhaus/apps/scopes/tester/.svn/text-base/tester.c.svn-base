#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "contiki.h"
#include "dev/leds.h"
#include "dev/button-sensor.h"
#include "lib/random.h"
#include "net/rime/ctimer.h"
#include "node-id.h"
#include "cfs.h"
#include "cfs-coffee.h"

#include "scopes-types.h"
#include "scopes.h"
#include "scopes-selfur.h"
#include "scopes-membership-simple.h"
#include "scopes-application.h"
#include "scopes-repository.h"
#include "scopes-logger.h"

#define SCOPES_TESTER_SID 1
#define ROOT_NODE_ID 1

/* super scope */
#define SUPER_SCOPE_ID 111
#define SUPER_SCOPE_TTL 60
#define SUPER_SCOPE_SPEC PREDICATE_LET, REPOSITORY_VALUE, NODE_ID, UINT8_VALUE, 5

/* sub scope */
#define SUB_SCOPE_ID 222
#define SUB_SCOPE_TTL 60
#define SUB_SCOPE_SPEC PREDICATE_GET, REPOSITORY_VALUE, NODE_ID, UINT8_VALUE, 4

PROCESS(tester_process, "scopes tester");
AUTOSTART_PROCESSES(&tester_process);

static unsigned char leds_status;

static struct subscriber scopes_tester;
static struct ctimer timer;
static char *test_data = "01234567890123456789012345678901234567890123456789";

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

void
send_reply(void *scope)
{
  struct scope *s = (struct scope *) scope;

  LOGF("(SCOPES TESTER) sending reply to scope %u\n", s->scope_id);
  leds_off(LEDS_RED);
  scopes_send(&scopes_tester, s, TRUE, test_data, strlen(test_data));
}

void
add_scope(struct scope *scope)
{
//  LOGF("(SCOPES TESTER) added scope: %u\n", scope->scope_id);
}

void
remove_scope(struct scope *scope)
{
//  LOGF("(SCOPES TESTER) removed scope: %u\n", scope->scope_id);
}

void
join_scope(struct scope *scope)
{
//  LOGF("(SCOPES TESTER) joined scope: %u\n", scope->scope_id);
  if (scope->scope_id == SUPER_SCOPE_ID) {
    leds_on(LEDS_BLUE);
  }
  else if (scope->scope_id == SUB_SCOPE_ID) {
    leds_on(LEDS_GREEN);
  }
}

void
leave_scope(struct scope *scope)
{
//  LOGF("(SCOPES TESTER) left scope: %u\n", scope->scope_id);
  if (scope->scope_id == SUPER_SCOPE_ID) {
    leds_off(LEDS_BLUE);
  }
  else if (scope->scope_id == SUB_SCOPE_ID) {
    leds_off(LEDS_GREEN);
  }
  leds_off(LEDS_RED);
  ctimer_stop(&timer);
}

void
recv_data(struct scope *scope, void *data, data_len_t data_len)
{
  char *msg = (char *) malloc(data_len+1);
  if (msg != NULL) {
    memcpy(msg, data, data_len);
    msg[data_len] = '\0';

    LOGF("(SCOPES TESTER) received data in scope %u: %s\n", scope->scope_id, msg);

    free(msg);

    if (node_id != ROOT_NODE_ID) {
      leds_on(LEDS_RED);
      uint8_t delay = 5 + (random_rand() % 5);
      ctimer_set(&timer, CLOCK_SECOND * delay, send_reply, (void *) scope);
      LOGF("(SCOPES TESTER) reply timer set to %d seconds\n", delay);
    }
  }
}

/* end scope functions */

static struct scopes_application tester_callbacks = { add_scope, remove_scope, join_scope, leave_scope, recv_data };

PROCESS_THREAD(tester_process, ev, data)
{
  PROCESS_BEGIN();

  leds_off(LEDS_ALL);

  LOGF("%s (node id: %u)\n", "tester application started", node_id);
	
  scopes_init(&scopes_selfur, &simple_membership);
  scopes_register(&scopes_tester, SCOPES_TESTER_SID, &tester_callbacks);

  static struct scope *super_scope;
//  static struct scope *sub_scope;
  static uint8_t super_spec[] = { SUPER_SCOPE_SPEC };
//  static uint8_t sub_spec[] = { SUB_SCOPE_SPEC };

  leds_off(LEDS_ALL);

  static struct etimer control_timer;
//    static struct etimer button_timer;

    LOG("(SCOPES TESTER) ready to handle commands\n");

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
        LOG("(SCOPES TESTER) opening super scope\n");
        super_scope = scopes_open(&scopes_tester, &world_scope, SUPER_SCOPE_ID,
                                  super_spec, sizeof(super_spec)/sizeof(uint8_t), SCOPES_FLAG_NONE, SUPER_SCOPE_TTL);

        /* wait a while */
        etimer_set(&control_timer, 10 * CLOCK_SECOND);
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&control_timer));

        LOG("(HEIGHT) %u\n", get_max_tree_height());

        /* wait a while */
        etimer_set(&control_timer, 100 * CLOCK_SECOND);
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&control_timer));


        /* wait a while */
        etimer_set(&control_timer, 10 * CLOCK_SECOND);
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&control_timer));


        /* open sub scope */
//        LOG("(SCOPES TESTER) opening sub scope\n");
 //       sub_scope = scopes_open(&scopes_tester, super_scope, SUB_SCOPE_ID,
 //                               sub_spec, sizeof(sub_spec)/sizeof(uint8_t), SCOPES_FLAG_NONE, SUB_SCOPE_TTL);

		
		LOG("(SCOPES TESTER) sending message to super scope\n"); 
        scopes_send(&scopes_tester, super_scope, FALSE, test_data, strlen(test_data)); 
		
        /* LOGF("(SCOPES TESTER) sending message to sub scope\n"); */
        /* scopes_send(&scopes_tester, sub_scope, FALSE, test_data, strlen(test_data)); */

        /* wait a while */

		etimer_set(&control_timer, 100 * CLOCK_SECOND);
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&control_timer));
        etimer_set(&control_timer, 100 * CLOCK_SECOND);
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&control_timer));

        /* close super scope */
        LOG("(SCOPES TESTER) closing super scope\n");
        scopes_close(&scopes_tester, super_scope);
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
