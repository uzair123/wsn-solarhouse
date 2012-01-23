#include "contiki.h"
#include <stdio.h>
#include "dev/sensorstation.h"
//#include "sensors.h"
//#include "dev/uart0.h"


/*----------------------------------------------------------------*/
PROCESS(sensor_test_process, "sensor_test_process");
AUTOSTART_PROCESSES(&sensor_test_process);
/*----------------------------------------------------------------*/
PROCESS_THREAD(sensor_test_process, ev, data)
{
	//struct _sensor_values results;
	static struct etimer et;

	PROCESS_BEGIN();
	//SENSORS_ACTIVATE(temperature_sensor);
	//SENSORS_ACTIVATE(humidity_sensor);
	sensorstation_init();
	//uart0_init(0);

	while(1) { 
		etimer_set(&et, CLOCK_SECOND * 1);
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

		sensorstation_read();
		//results = sensorstation_read();
		//printf("temperature: %d C/1000 , humidity: %d%%/1000\r\n", results.temperature, results.humidity);
	}
	PROCESS_END();
}
/*----------------------------------------------------------------*/
