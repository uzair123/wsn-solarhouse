/* This is a simple example showing the usage of the scopes API. The
   example goes through a typical sequence of opening, using, and
   closing a scope. The sequence can be advanced by pushing the button
   on the sensor node. The example creates a scope in which the
   reading from the temperature sensor must be above 25 C and the TSR
   light value must be under (heat up the mote under a lamp and move
   it into a dark spot to make it join the scope) */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "contiki.h"
#include "dev/leds.h"
#include "dev/button-sensor.h"

#include "scopes-types.h"       /* scope types */
#include "scopes.h"             /* the basic scopes header */
#include "scopes-flooding.h"    /* flooding is used in this example */
#include "scopes-membership.h" /* simple membership for membership tests */
#include "scopes-application.h"       /* application interface */

#define APP_EXAMPLE_SID 1       /* subscriber id of the example
                                   application */

PROCESS(example, "scopes example");
AUTOSTART_PROCESSES(&example);

/* Each contiki process that wants to use scopes must provide a
   subscriber structure. This structure stores all information about a
   subscribed process, including its callback functions and a unique
   subscriber id (sid). The subscriber id is used to address a process
   in a scopes message. It is required because contiki processes have
   no explicit process ids. */
static struct subscriber application;


/* ---- application callback functions ---- */

/* These functions are called by scopes to signify that some event has
   occured. */

/* called when the node adds a scope */
void
add_scope(struct scope *scope)
{
  printf("APPLICATION: the node has added scope: %u\n", scope->scope_id);
}

/* called when the node removes a scope */
void
remove_scope(struct scope *scope)
{
  printf("APPLICATION: the node has removed scope: %u\n", scope->scope_id);
}

/* called when the node joins a scope */
void
join_scope(struct scope *scope)
{
  printf("APPLICATION: the node has joined scope: %u\n", scope->scope_id);
  leds_on(LEDS_GREEN);
}

/* called when the node leaves a scope */
void
leave_scope(struct scope *scope)
{
  printf("APPLICATION: the node has left scope: %u\n", scope->scope_id);
  leds_off(LEDS_ALL);
}

/* called when data for this subscriber is received in a scope */
void
recv_data(struct scope *scope, void *data, data_len_t data_len)
{
  char *msg = (char *) malloc(data_len+1);
  if (msg != NULL) {
    memcpy(msg, data, data_len);
    msg[data_len] = '\0';

    printf("APPLICATION: received data in scope %u: %s\n", scope->scope_id, msg);
    leds_on(LEDS_RED);
  }
}

/* This structure collects the pointers to the callback functions and
   is passed to scopes when the process subscribes. */
static struct scopes_application application_callbacks = { add_scope, remove_scope, join_scope, leave_scope, recv_data };


/* ---- application process ---- */
PROCESS_THREAD(example, ev, data)
{
  PROCESS_BEGIN();

  leds_off(LEDS_ALL);

  /* Initialize scopes with a particular routing and membership
     testing. This must be done exactly once before any other scope
     functions are called. */
  scopes_init(&scopes_flooding, &simple_membership);

  /* Register with scopes. Pass a reference to the subscriber
     structure, a unique subscriber id used to address the
     application, and the application's callback functions. */
  scopes_register(&application, APP_EXAMPLE_SID, &application_callbacks);

  while(1) {
    /* wait for button push */
    PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);

    /* Create a structure reference for the new scope. The pointer is
       declared static in this example so it won't be allocated on the
       stack (this is due to protothreads) */
    static struct scope *light_scope;

    /* Create the scope specification */
    static uint8_t spec[] = { 1, 2, 3 };

    /* Open the scope. Pass a reference to the subscriber structure, a
       reference to the scope structure of the super scope, a unique
       scope id for the new scope, a reference to the scope
       specification, the length of the scope specification, scope
       flags, and the lifetime of the scope in seconds. */
    printf("opening scope\n");
    light_scope = scopes_open(&application, &world_scope, 123, spec, sizeof(spec)/sizeof(uint8_t), SCOPES_FLAG_DYNAMIC, 30);

    /* wait for button push */
    PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);

    char *application_data = "this is a message from the creator";

    /* Send data to the scope. Pass a reference to the subscriber
       structure, a reference to the scope structure, the subscriber
       id of the remote application, a reference to the data to be
       sent, and the length of the data */
    printf("sending message\n");
    scopes_send(&application, light_scope, TRUE, application_data, strlen(application_data));

    /* wait for button push */
    PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);

    /* Close the scope. Pass a reference to the subscriber structure
       and a reference to the scope structure. */
    printf("closing scope\n");
    scopes_close(&application, light_scope);
  }

  PROCESS_END();
}
