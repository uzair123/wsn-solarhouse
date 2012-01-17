#include "contiki.h"
#include <stdio.h>
#include "sensors.h"
#include "dev/tmp102.h"
#include "dev/co-sensor.h"
#include "dev/humidity-sensor.h"
#include "dev/temperature-sensor.h"
#include "dev/light-sensor.h"


#define LIGHT_SENSOR_TOTAL_SOLAR 1

/*----------------------------------------------------------------*/
PROCESS(sensor_test_process, "sensor_test_process");
AUTOSTART_PROCESSES(&sensor_test_process);
/*----------------------------------------------------------------*/
PROCESS_THREAD(sensor_test_process, ev, data)
{
	static uint16_t temperature, light, co2, humidity;
	static struct etimer et;
	static int8_t temp2;

	PROCESS_BEGIN();
	SENSORS_ACTIVATE(temperature_sensor);
	SENSORS_ACTIVATE(light_sensor);
	SENSORS_ACTIVATE(co_sensor);
	SENSORS_ACTIVATE(humidity_sensor);
	tmp102_init();
	
	while(1) { 
		// 30 seconds timer
		etimer_set(&et, CLOCK_SECOND * 1);
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

		temperature = temperature_sensor.value(0);
		temp2 = tmp102_read_temp_simple();
		light = light_sensor.value(LIGHT_SENSOR_TOTAL_SOLAR);
		co2 = co_sensor.value(0);
		humidity = humidity_sensor.value(0);
		
		
		printf("temperature: %d temperatur(tmp102): %d light: %d co2: %d humidity: %d \r\n", 
			temperature, temp2, light, co2, humidity);
	}
	PROCESS_END();
}
/*----------------------------------------------------------------*/
