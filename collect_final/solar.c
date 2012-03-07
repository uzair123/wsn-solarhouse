#include "contiki.h"
#include "lib/random.h"

#include "net/rime/collect.h"

#include "dev/battery-sensor.h"
#include "dev/humidity-sensor.h"
#include "dev/tmp102.h"
//#include "dev/sensorstation.h"
#include "sensors.h"

#include "sys/energest.h"

#include "dev/cc2420.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*---------------------------------------------------------------------------*/
//#define SINK 1
#define SEND_INTERVAL_SECONDS 60		// standard: 60
#define SETTLE_TIMEOUT_SECONDS 10		// standard: 120
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
	unsigned long cpu_energy, lpm_energy, rx_energy, tx_energy, red_energy, green_energy, yellow_energy;
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

	printf("t: %lu, from %d.%d, hops: %d, rssi: %d seq %d, co2: %d temp: %d hum: %d temp(z1): %d hum(z1): %d bat(z1): %d ", 
		clock_seconds(),
		originator->u8[0], originator->u8[1], hops, rssi, seqno,
		data->co2_station,
		data->temp_station,
		data->humidity_station,
		data->temp_z1, 
		data->humidity_z1,
		data->battery);

	printf("cpu: %lu lpm: %lu rx: %lu tx: %lu red: %lu grn: %lu ylwo: %lu\r\n",data->cpu_energy, data->lpm_energy, data->rx_energy, data->tx_energy, data->red_energy , data->green_energy , data->yellow_energy);
}
/*---------------------------------------------------------------------------*/
static const struct collect_callbacks callbacks = { recv };
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(solarhouse_collect_process, ev, data)
{
    static struct etimer periodic;
    static struct etimer et;
  
	//rx = receive(listen), tx = transmit
	static unsigned long cpu_start_time, lpm_start_time, rx_start_time, tx_start_time, led_red_start_time, led_green_start_time, led_yellow_start_time;
	static unsigned long cpu_delta, lpm_delta, rx_delta, tx_delta, red_delta, green_delta, yellow_delta;
	static unsigned long cpu_acc=0, lpm_acc=0, rx_acc=0, tx_acc=0, red_acc=0, green_acc=0, yellow_acc=0, energy_used;
	static unsigned long cpu_energy, lpm_energy, rx_energy, tx_energy, red_energy, green_energy, yellow_energy;

    PROCESS_BEGIN();
    cc2420_set_txpower(CC2420_TXPOWER);

    /* initialize sensors */
	//sensorstation_init();
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
		//energy measurements start
		cpu_start_time = energest_type_time(ENERGEST_TYPE_CPU); //computation
		lpm_start_time = energest_type_time(ENERGEST_TYPE_LPM); //sleeping
		rx_start_time = energest_type_time(ENERGEST_TYPE_LISTEN);
		tx_start_time = energest_type_time(ENERGEST_TYPE_TRANSMIT);
		led_red_start_time = energest_type_time(ENERGEST_TYPE_LED_RED); //leds:
		led_green_start_time = energest_type_time(ENERGEST_TYPE_LED_GREEN);
		led_yellow_start_time = energest_type_time(ENERGEST_TYPE_LED_YELLOW);


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

			data.cpu_energy = cpu_energy;		
			data.lpm_energy = lpm_energy;
			data.rx_energy = rx_energy;
			data.tx_energy = tx_energy;
			data.red_energy = red_energy;
			data.green_energy = green_energy;
			data.yellow_energy = yellow_energy;

			packetbuf_clear();
			packetbuf_copyfrom(&data, sizeof(struct solarhouse_sensor_data));
			collect_send(&tc, NUM_RETRANSMITS);

			/*
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
			*/
		#endif
		#ifdef SINK
			// read and print sensor values, without sending them
			struct solarhouse_sensor_data data = read_sensors();

			data.cpu_energy = cpu_energy;		
			data.lpm_energy = lpm_energy;
			data.rx_energy = rx_energy;
			data.tx_energy = tx_energy;
			data.red_energy = red_energy;
			data.green_energy = green_energy;
			data.yellow_energy = yellow_energy;

			printf("t: %lu, from %d.%d, hops: %d, rssi: %d seq %d, co2: %d temp: %d hum: %d temp(z1): %d hum(z1): %d bat(z1): %d ", 
					clock_seconds(),
					rimeaddr_node_addr.u8[0], rimeaddr_node_addr.u8[1], 0, 0, 0,
					data.co2_station,
					data.temp_station,
					data.humidity_station,
					data.temp_z1, 
					data.humidity_z1,
					data.battery);
			printf("cpu: %lu lpm: %lu rx: %lu tx: %lu red: %lu grn: %lu ylwo: %lu\r\n",data.cpu_energy, data.lpm_energy, data.rx_energy, data.tx_energy, data.red_energy , data.green_energy , data.yellow_energy);
		#endif
        }

		//energy measurements (so many variables just for the printf)
		cpu_delta = energest_type_time(ENERGEST_TYPE_CPU) - cpu_start_time;
		lpm_delta = energest_type_time(ENERGEST_TYPE_LPM) - lpm_start_time;
		rx_delta = energest_type_time(ENERGEST_TYPE_LISTEN) - rx_start_time;
		tx_delta = energest_type_time(ENERGEST_TYPE_TRANSMIT) - tx_start_time;
		red_delta = energest_type_time(ENERGEST_TYPE_LED_RED) - led_red_start_time;
		green_delta = energest_type_time(ENERGEST_TYPE_LED_GREEN) - led_green_start_time;
		yellow_delta = energest_type_time(ENERGEST_TYPE_LED_YELLOW) - led_yellow_start_time;

		cpu_acc += cpu_delta;
		lpm_acc += lpm_delta;
		rx_acc += rx_delta;
		tx_acc += tx_delta;
		green_acc += green_delta;
		red_acc += red_delta;
		yellow_acc += yellow_delta;

		cpu_energy = (unsigned long) (1.8 * cpu_acc * 3 / (unsigned long) RTIMER_SECOND);
		lpm_energy = (unsigned long) (0.0545 * lpm_acc * 3 / (unsigned long) RTIMER_SECOND);
		rx_energy = (unsigned long) (20 * rx_acc * 3 / (unsigned long) RTIMER_SECOND);
		tx_energy = (unsigned long) (17.4 * tx_acc * 3 / (unsigned long) RTIMER_SECOND);
		red_energy = (unsigned long) (25.0 * red_acc * 3 / (unsigned long) RTIMER_SECOND);
		green_energy = (unsigned long) (20.0 * green_acc * 3 / (unsigned long) RTIMER_SECOND);
		yellow_energy = (unsigned long) (25.0 * yellow_acc * 3 / (unsigned long) RTIMER_SECOND);
		
		//energy_used = cpu_energy + lpm_energy + rx_energy + tx_energy + red_energy + green_energy + yellow_energy;

		//printf("e %lu cpu %lu lpm %lu rx %lu tx %lu red %lu green %lu ylwo %lu\n",energy_used, cpu_energy, lpm_energy, rx_energy, tx_energy, red_energy , green_energy , yellow_energy);

    }
    
    PROCESS_END();
}
/*---------------------------------------------------------------------------*/
static struct solarhouse_sensor_data read_sensors()
{
	struct solarhouse_sensor_data data;
	memset(&data, 0, sizeof(struct solarhouse_sensor_data));
	
	// get data from sensor stations
	/*struct sensor_values station_results = 	sensorstation_read();
	data.co2_station = 			station_results.co2;
	data.temp_station = 		station_results.temperature;
	data.humidity_station = 	station_results.humidity;
	*/
	data.co2_station = 		0;
	data.temp_station = 		0;
	data.humidity_station = 	0;

	// get data from z1 sensors
	data.temp_z1 = 			tmp102_read_temp_simple();
	data.humidity_z1 = 		humidity_sensor.value(0);
	data.battery = 			battery_sensor.value(0);
    
	return data;
}
