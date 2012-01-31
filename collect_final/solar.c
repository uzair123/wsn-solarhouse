#include "contiki.h"
#include "lib/random.h"

#include "net/rime/collect.h"

#include "dev/battery-sensor.h"
#include "dev/humidity-sensor.h"
#include "dev/tmp102.h"
#include "dev/sensorstation.h"
#include "sensors.h"

#include "dev/cc2420.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*---------------------------------------------------------------------------*/
#define SINK 1
#define SEND_INTERVAL_SECONDS 60		// standard: 60
#define SETTLE_TIMEOUT_SECONDS 5		// standard: 120
#define COLLECT_CHANNEL 130			// standard: 130
#define NUM_RETRANSMITS 15			// standard: 15
#define CC2420_TXPOWER 31			// standard: Maximum = 31
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
static struct collect_conn tc;
/*---------------------------------------------------------------------------*/
PROCESS(solarhouse_collect_process, "Solarhouse Collect");
AUTOSTART_PROCESSES(&solarhouse_collect_process);
/*---------------------------------------------------------------------------*/
static void recv(const rimeaddr_t *originator, uint8_t seqno, uint8_t hops)
{
	//data is already on the mote, copying of the data is not necessary
	struct solarhouse_sensor_data *data = packetbuf_dataptr();
	int16_t rssi = packetbuf_attr(PACKETBUF_ATTR_RSSI);

	printf("time: %lu, from %d.%d, hops: %d, rssi: %d seqno %d, co2: %d temp: %d humidity: %d temp(z1): %d humidity(z1): %d battery(z1): %d\r\n", 
		clock_seconds(),
		originator->u8[0], originator->u8[1], hops, rssi, seqno,
		data->co2_station,
		data->temp_station,
		data->humidity_station,
		data->temp_z1, 
		data->humidity_z1,
		data->battery);
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
	sensorstation_init();
	SENSORS_ACTIVATE(battery_sensor);
	SENSORS_ACTIVATE(humidity_sensor);
	tmp102_init();

    collect_open(&tc, COLLECT_CHANNEL, COLLECT_ROUTER, &callbacks);

	#ifdef SINK
		printf("i'am sink, addr: %d\r\n", rimeaddr_node_addr.u8[0]);
		collect_set_sink(&tc, 1);
	#endif

	printf("Time for network to settle: %d sec, Send intervall: %d sec\r\n", SETTLE_TIMEOUT_SECONDS, SEND_INTERVAL_SECONDS);
 
    /* Allow some time for the network to settle. */
    etimer_set(&et, SETTLE_TIMEOUT_SECONDS * CLOCK_SECOND);
    PROCESS_WAIT_UNTIL(etimer_expired(&et));

    while(1) 
    {
        /* Send a packet every SEND_INTERVAL_SECONDS + rand(0, SEND_INTERVAL_SECONDS-1) seconds. */
        if(etimer_expired(&periodic)) 
        {
            etimer_set(&periodic, CLOCK_SECOND * SEND_INTERVAL_SECONDS);
            etimer_set(&et, random_rand() % (CLOCK_SECOND * SEND_INTERVAL_SECONDS)); //always faster than the periodic timer
        }

        PROCESS_WAIT_EVENT();

        // send sensor data
        if(etimer_expired(&et))
        {
		#ifndef SINK	//read sensors & SEND
			struct solarhouse_sensor_data data = read_sensors();
			packetbuf_clear();
			packetbuf_copyfrom(&data, sizeof(struct solarhouse_sensor_data));
			collect_send(&tc, NUM_RETRANSMITS);

			//print Tree
			static rimeaddr_t oldparent;
			const rimeaddr_t *parent;
			parent = collect_parent(&tc);
			//if parent changed
			if(!rimeaddr_cmp(parent, &oldparent)) {
				//alten parent ausgeben (falls der nicht null war)
				if(!rimeaddr_cmp(&oldparent, &rimeaddr_null)) {
				  printf("#L %d is old parent\r\n", oldparent.u8[0]);
				}
				//neuen parent ausgeben (falls der nicht null ist)
				if(!rimeaddr_cmp(parent, &rimeaddr_null)) {
				  printf("#L %d is new parent\r\n", parent->u8[0]);
				}
				//speichern
				rimeaddr_copy(&oldparent, parent);
			}
		#endif
		#ifdef SINK
			// read and print sensor values, without sending them
			struct solarhouse_sensor_data data = read_sensors();
			printf("time: %lu, from %d.%d, hops: %d, rssi: %d seqno %d, co2: %d temp: %d humidity: %d temp(z1): %d humidity(z1): %d battery(z1): %d\r\n", 
					clock_seconds(),
					rimeaddr_node_addr.u8[0], rimeaddr_node_addr.u8[1], 0, 0, 0,
					data.co2_station,
					data.temp_station,
					data.humidity_station,
					data.temp_z1, 
					data.humidity_z1,
					data.battery);
		#endif
        }

    }
    
    PROCESS_END();
}
/*---------------------------------------------------------------------------*/
static struct solarhouse_sensor_data read_sensors()
{
	struct solarhouse_sensor_data data;
	memset(&data, 0, sizeof(struct solarhouse_sensor_data));
	
	// get data from sensor stations
	struct sensor_values station_results = 	sensorstation_read();
	data.co2_station = 			station_results.co2;
	data.temp_station = 		station_results.temperature;
	data.humidity_station = 	station_results.humidity;
	
	// get data from z1 sensors
	data.temp_z1 = 			tmp102_read_temp_simple();
	data.humidity_z1 = 		humidity_sensor.value(0);
	data.battery = 			battery_sensor.value(0);
    
	return data;
}
