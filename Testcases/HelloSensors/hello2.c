#include "contiki.h"
#include <stdio.h>
#include "sensors.h"
#include "dev/tmp102.h"


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
	
	tmp102_init();
	
	while(1) { 
		// 30 seconds timer
		etimer_set(&et, CLOCK_SECOND * 1);
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

		temp2 = tmp102_read_temp_simple();		
		
		printf("temperature: %d \r\n", 
			temp2);
	}
	PROCESS_END();
}
/*----------------------------------------------------------------*/
