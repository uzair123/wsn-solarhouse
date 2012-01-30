#include "contiki.h"
#include <stdio.h>
#include "sensors.h"
#include "dev/light-sensor.h"



//LIGHT_SENSOR_TOTAL_SOLAR

/*----------------------------------------------------------------*/
PROCESS(sensor_test_process, "sensor_test_process");
AUTOSTART_PROCESSES(&sensor_test_process);
/*----------------------------------------------------------------*/
PROCESS_THREAD(sensor_test_process, ev, data)
{

	static struct etimer et;

	PROCESS_BEGIN();
	
	//SENSORS_ACTIVATE(light_sensor);
	light_sensor.configure(SENSORS_ACTIVE, 1);

	while(1) { 
		etimer_set(&et, CLOCK_SECOND * 1);
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
		
		uint16_t light = light_sensor.value(0);	
		uint16_t light2 = light_sensor.value(1);
	
		printf("light: %d , %d\r\n", 
			light, light2);
	}
	PROCESS_END();
}
/*----------------------------------------------------------------*/
