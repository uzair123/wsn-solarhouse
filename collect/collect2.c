#include "contiki.h"
//#include "lib/random.h"

//#include "net/rime.h"
#include "net/rime/collect.h"

//#include "dev/battery-sensor.h"
//#include "dev/humidity-sensor.h"
//#include "dev/tmp102.h"
//#include "dev/co-sensor.h"
//#include "dev/sensorstation.h"
//#include "sensors.h"

//#include "dev/cc2420.h"
//#include <stdio.h>
//include <stdlib.h>
//#include <string.h>
/*---------------------------------------------------------------------------*/
/*#define SEND_INTERVAL_SECONDS 30
#define SETTLE_TIMEOUT_SECONDS 120
#define COLLECT_CHANNEL_x 130
#define NUM_RETRANSMITS_x 15
// we use maximum transmission power to compare it with the final implementation
#define CC2420_TXPOWER_x 31*/
/*---------------------------------------------------------------------------*/
PROCESS(solarhouse_collect_process, "Solarhouse Collect");
AUTOSTART_PROCESSES(&solarhouse_collect_process);
/*---------------------------------------------------------------------------*/
static struct collect_conn tc;
/*---------------------------------------------------------------------------*/
//according to pablos mail
struct solarhouse_sensor_data{
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
static void recv(const rimeaddr_t *originator, uint8_t seqno, uint8_t hops){

	//data is already on the mote, copying of the data is not necessary
	struct solarhouse_sensor_data *data = packetbuf_dataptr();

	printf("light: %d temperature: %d co2: %d humidity: %d battery: %d\n", 
	data.light, 
	data.temperature, 
	data.co2, 
	data.humidity,
	data.battery);
}
/*---------------------------------------------------------------------------*/
static const struct collect_callbacks callbacks = { recv };
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(solarhouse_collect_process, ev, data){
    static struct etimer periodic;
    static struct etimer et;
  
    PROCESS_BEGIN();

    //cc2420_set_txpower(CC2420_TXPOWER_x);

    /* initialize sensors 
		// DEPENDING ON WHICH DEVICE!! or not?
	SENSORS_ACTIVATE(battery_sensor);
	SENSORS_ACTIVATE(humidity_sensor);

	tmp102_init();
    /* ... */

    collect_open(&tc, COLLECT_CHANNEL_x, COLLECT_ROUTER, &callbacks);

#ifdef SINK
    printf("i'am sink\n");
    collect_set_sink(&tc, 1);
#endif
 
    /* Allow some time for the network to settle. */
    etimer_set(&et, SETTLE_TIMEOUT_SECONDS * CLOCK_SECOND);
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
            //struct solarhouse_sensor_data data2 = read_sensors();
            // TODO read sensors
            collect_send(&tc, NUM_RETRANSMITS_x);
        }
#endif
    }
    
    PROCESS_END();
}
/*---------------------------------------------------------------------------*/
static struct solarhouse_sensor_data read_sensors(){
/*
	struct solarhouse_sensor_data data;
	memset(&data, 0, sizeof(struct solarhouse_sensor_data));

//#ifdef ...
	data.co2_station = NULL;
	data.temp_station = NULL;
	data.humidity_station = NULL;
	data.temp_z1 = tmp102_read_temp_simple();
	data.humidity_z1 = humidity_sensor.value(0);
	data.battery = battery_sensor.value(0);
    
	return data;*/
}
