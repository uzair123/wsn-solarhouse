#include "contiki.h"
#include <stdio.h>
#include "dev/temperature-sensor.h"
#include "dev/humidity-sensor.h"
#include "sensors.h"

/*----------------------------------------------------------------*/
PROCESS(sensor_test_process, "sensor_test_process");
AUTOSTART_PROCESSES(&sensor_test_process);
/*----------------------------------------------------------------*/
PROCESS_THREAD(sensor_test_process, ev, data)
{
	static uint16_t temperature, humidity;
	static struct etimer et;

	PROCESS_BEGIN();
	SENSORS_ACTIVATE(temperature_sensor);
	SENSORS_ACTIVATE(humidity_sensor);

	while(1) { 
		etimer_set(&et, CLOCK_SECOND * 1);
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

		temperature = temperature_sensor.value(0);
		humidity = humidity_sensor.value(0);
		
		printf("temperature: %d C/1000 , humidity: %d%%/1000\r\n", temperature, humidity);
	}
	PROCESS_END();
}
/*----------------------------------------------------------------*/
