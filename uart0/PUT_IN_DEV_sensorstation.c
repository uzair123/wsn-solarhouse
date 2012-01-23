#include <stdio.h>
#include <signal.h>
#include "contiki.h"
#include "sensorstation.h"
#include "dev/uart0.h"

static int16_t co2, humidity, temperature;
static int counter;
static int uart0_input_handler(unsigned char c){
	if(c == 255){
		counter = 0;
		return 0;
	}
	switch(counter)
	{
	   case 0:
		_sensor_values.co2 = c << 8;
	   break;
	   case 1:
		_sensor_values.co2 = _sensor_values.co2 | c;
	   break;
	   case 2:
		_sensor_values.humidity = c << 8;
	   break;
	   case 3:
		_sensor_values.humidity = _sensor_values.humidity | c;;
	   break;
	   case 4:
		_sensor_values.temperature = c << 8;
	   break;
	   case 5:
		_sensor_values.temperature = _sensor_values.temperature | c;
	   break;
	}	
	counter++;
	return 0;
}

void
sensorstation_init (void)
{
  
  uart0_init(0);
  uart0_set_input(&uart0_input_handler);
}


struct sensor_values
sensorstation_read()
{	
  return _sensor_values;
}
