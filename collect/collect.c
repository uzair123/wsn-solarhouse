#include "contiki.h"
#include "lib/random.h"
#include "net/rime.h"
#include "net/rime/collect.h"
#include "dev/leds.h"
#include "dev/button-sensor.h"

#include "net/netstack.h"

#include <stdio.h>

//#define SINK 1
#define SEND_INTERVAL_SECONDS 30
#define COLLECT_CHANNEL 130

static struct collect_conn tc;

struct collect_data{
    int light;
    uint8_t temp;
    int co2;
    int humidity;
}

/*---------------------------------------------------------------------------*/
PROCESS(solarhouse_collect_process, "Solarhouse Collect");
AUTOSTART_PROCESSES(&solarhouse_collect_process);
/*---------------------------------------------------------------------------*/
static void
recv(const rimeaddr_t *originator, uint8_t seqno, uint8_t hops){
    struct collect_data data;
    memcopy(&data, packetbuf_dataptr(), sizeof(struct collect_data));
    
    printf("light: %d temp: %u co2: %d humidity: %d\n", 
        data.light, 
        data.temp, 
        data.co2, 
        data.humidity);
}
/*---------------------------------------------------------------------------*/
static const struct collect_callbacks callbacks = { recv };
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(solarhouse_collect_process, ev, data){
  static struct etimer periodic;
  static struct etimer et;
  
  PROCESS_BEGIN();

  collect_open(&tc, COLLECT_CHANNEL, COLLECT_ROUTER, &callbacks);

#ifdef SINK
  printf("i'am sink\n");
  collect_set_sink(&tc, 1);
#endif 
 
  /* Allow some time for the network to settle. */
  etimer_set(&et, 120 * CLOCK_SECOND);
  PROCESS_WAIT_UNTIL(etimer_expired(&et));

  while(1) {

    /* Send a packet every SEND_INTERVAL_SECONDS seconds. */
    if(etimer_expired(&periodic)) {
        etimer_set(&periodic, CLOCK_SECOND * SEND_INTERVAL_SECONDS);
        etimer_set(&et, random_rand() % (CLOCK_SECOND * SEND_INTERVAL_SECONDS));
    }

    PROCESS_WAIT_EVENT();

#ifndef SINK
    // send sensor data
    if(etimer_expired(&et)) {
        // TODO read sensors
        collect_send(&tc, 15);
    }
#endif
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/