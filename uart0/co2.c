#include "contiki.h"
#include <stdio.h>
#include "dev/sensorstation.h"

/*----------------------------------------------------------------*/
PROCESS(sensor_test_process, "sensor_test_process");
AUTOSTART_PROCESSES(&sensor_test_process);
/*----------------------------------------------------------------*/
PROCESS_THREAD(sensor_test_process, ev, data)
{
	struct sensor_values results;
	static struct etimer et;

	PROCESS_BEGIN();
	sensorstation_init();

	while(1) { 
		etimer_set(&et, CLOCK_SECOND * 1);
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

		results = sensorstation_read();
		printf("CO2: %d ,temperature: %d, humidity: %d \r\n", results.co2, results.temperature, results.humidity);
	}
	PROCESS_END();
}
/*----------------------------------------------------------------*/
