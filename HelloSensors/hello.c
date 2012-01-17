#include "contiki.h"
#include <stdio.h>
#include "dev/temperature-sensor.h"
#include "dev/light-sensor.h"
#include "sensors.h"

#define LIGHT_SENSOR_TOTAL_SOLAR 1

/*----------------------------------------------------------------*/
PROCESS(sensor_test_process, "sensor_test_process");
AUTOSTART_PROCESSES(&sensor_test_process);
/*----------------------------------------------------------------*/
PROCESS_THREAD(sensor_test_process, ev, data)
{
	static uint16_t temperature, light;
	static struct etimer et;

	PROCESS_BEGIN();
	SENSORS_ACTIVATE(temperature_sensor);
	SENSORS_ACTIVATE(light_sensor);

	while(1) { 
		// 30 seconds timer
		etimer_set(&et, CLOCK_SECOND * 1);
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

		temperature = temperature_sensor.value(0);
		light = light_sensor.value(LIGHT_SENSOR_TOTAL_SOLAR);
		printf("temperature: %d , light %d\r\n", temperature, light);
	}
	PROCESS_END();
}
/*----------------------------------------------------------------*/
