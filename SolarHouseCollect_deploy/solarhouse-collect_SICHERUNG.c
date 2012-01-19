#include "contiki.h"
#include "lib/random.h"

#include "net/rime.h"
#include "net/rime/collect.h"

#include "dev/battery-sensor.h"
#include "dev/humidity-sensor.h"
#include "dev/tmp102.h"
//#include "dev/sensorstation.h"
#include "sensors.h"

#include "dev/cc2420.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*---------------------------------------------------------------------------*/
#define SINK 1
#define SEND_INTERVAL_SECONDS 5		// standard: 30
#define SETTLE_TIMEOUT_SECONDS 5		// standard: 120
#define COLLECT_CHANNEL 130				// standard: 130
#define NUM_RETRANSMITS 15				// standard: 15
// we use maximum transmission power to compare it with the final implementation
#define CC2420_TXPOWER 31
/*---------------------------------------------------------------------------*/
PROCESS(solarhouse_collect_process, "Solarhouse Collect");
AUTOSTART_PROCESSES(&solarhouse_collect_process);
/*---------------------------------------------------------------------------*/
static struct collect_conn tc;
/*---------------------------------------------------------------------------*/
struct solarhouse_sensor_data
{
    int16_t co2_station;
    int16_t temp_station;
    int16_t humidity_station;
    int16_t temp_z1;
    int16_t humidity_z1;
    int16_t battery;
};
/*---------------------------------------------------------------------------*/
static struct solarhouse_sensor_data read_sensors();
/*---------------------------------------------------------------------------*/
static void recv(const rimeaddr_t *originator, uint8_t seqno, uint8_t hops)
{
#ifdef SINK
	if(originator->u8[0] == rimeaddr_node_addr.u8[0]) return;
	//data is already on the mote, copying of the data is not necessary
	struct solarhouse_sensor_data *data = packetbuf_dataptr();

	printf("from %d.%d, co2: %d temp: %d humidity: %d temp(z1): %d humidity(z1): %d battery: %d\r\n", 
		originator->u8[0], originator->u8[1],
		data->co2_station,
		data->temp_station,
		data->humidity_station,
		data->temp_z1, 
		data->humidity_z1,
		data->battery);
#endif
}
/*---------------------------------------------------------------------------*/
static const struct collect_callbacks callbacks = { recv };
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(solarhouse_collect_process, ev, data)
{
    static struct etimer periodic;
    static struct etimer et;
  
    PROCESS_BEGIN();

    cc2420_set_txpower(CC2420_TXPOWER);

    /* initialize sensors */
	SENSORS_ACTIVATE(battery_sensor);
	SENSORS_ACTIVATE(humidity_sensor);
	tmp102_init();

    collect_open(&tc, COLLECT_CHANNEL, COLLECT_ROUTER, &callbacks);

#ifdef SINK
    printf("i'am sink\n");
    collect_set_sink(&tc, 1);
#endif
 
    /* Allow some time for the network to settle. */
    etimer_set(&et, SETTLE_TIMEOUT_SECONDS * CLOCK_SECOND);
    PROCESS_WAIT_UNTIL(etimer_expired(&et));

    while(1) 
    {
        /* Send a packet every SEND_INTERVAL_SECONDS seconds. */
        if(etimer_expired(&periodic)) 
        {
            etimer_set(&periodic, CLOCK_SECOND * SEND_INTERVAL_SECONDS);
            etimer_set(&et, random_rand() % (CLOCK_SECOND * SEND_INTERVAL_SECONDS));
        }

        PROCESS_WAIT_EVENT();

#ifndef SINK
        // send sensor data, build tree
        if(etimer_expired(&et))
        {

				//read sensors & SEND
		        struct solarhouse_sensor_data data = read_sensors();
				packetbuf_clear();
				packetbuf_copyfrom(&data, sizeof(struct solarhouse_sensor_data));
		        collect_send(&tc, NUM_RETRANSMITS);

			/*#ifdef SINK
				packetbuf_clear();
				packetbuf_set_datalen(sprintf(packetbuf_dataptr(),
							  "%s", "Hello") + 1);
				collect_send(&tc, 15);
			#endif*/

			//build tree
			static rimeaddr_t oldparent;
			const rimeaddr_t *parent;
			parent = collect_parent(&tc);
			if(!rimeaddr_cmp(parent, &oldparent)) {
			//alten parent ausgeben (falls der nicht null war)
			if(!rimeaddr_cmp(&oldparent, &rimeaddr_null)) {
				  printf("#L %d 0\n", oldparent.u8[0]);
			}
			//neuen parent ausgeben (falls der nicht null ist)
			if(!rimeaddr_cmp(parent, &rimeaddr_null)) {
				  printf("#L %d 1\n", parent->u8[0]);
			}
			//speichern
				rimeaddr_copy(&oldparent, parent);
			}
        }
#endif
    }
    
    PROCESS_END();
}
/*---------------------------------------------------------------------------*/
static struct solarhouse_sensor_data read_sensors()
{

	struct solarhouse_sensor_data data;
	memset(&data, 0, sizeof(struct solarhouse_sensor_data));
	
	// TODO
	// wait for a solution to compiler error (UART...) until sensor station
	// data can be read
	data.co2_station = NULL;
	data.temp_station = NULL;
	data.humidity_station = NULL;
	
	// get data from 
	data.temp_z1 = tmp102_read_temp_simple();
	data.humidity_z1 = humidity_sensor.value(0);
	data.battery = battery_sensor.value(0);
    
	return data;
}
