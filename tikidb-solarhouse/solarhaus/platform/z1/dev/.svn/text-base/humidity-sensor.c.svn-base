#include "contiki.h"
#include "lib/sensors.h"
#include "dev/humidity-sensor.h"
#include "dev/z1-sensors.h"

#include <io.h>

/* Photodiode on INCH_X */
#define INPUT_CHANNEL      (1 << INCH_1)//Aendern!!
#define INPUT_REFERENCE     SREF_0
#define HUMIDITY_MEM  ADC12MEM1		//Aendern!!

const struct sensors_sensor humidity_sensor;

/*---------------------------------------------------------------------------*/
static int
value(int type)
{
	return HUMIDITY_MEM * 2.7;
}
/*---------------------------------------------------------------------------*/
static int
status(int type)
{
  return z1_sensors_status(INPUT_CHANNEL, type);
}
/*---------------------------------------------------------------------------*/
static int
configure(int type, int c)
{
  return z1_sensors_configure(INPUT_CHANNEL, INPUT_REFERENCE, type, c);
}
/*---------------------------------------------------------------------------*/
SENSORS_SENSOR(humidity_sensor, "Humidity", value, configure, status);

