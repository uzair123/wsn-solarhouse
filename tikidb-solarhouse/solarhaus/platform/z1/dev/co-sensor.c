#include "contiki.h"
#include "lib/sensors.h"
#include "dev/co-sensor.h"
#include "dev/z1-sensors.h"

#include <io.h>

/* CO-Sensor on INCH_X */
#define INPUT_CHANNEL      (1 << INCH_7)//Aendern!!
#define INPUT_REFERENCE     SREF_0
#define TOTAL_CO_MEM  ADC12MEM7		//Aendern!!

const struct sensors_sensor co_sensor;

/*---------------------------------------------------------------------------*/
static int
value(int type)
{
	return CO_MEM;
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
SENSORS_SENSOR(co_sensor, "CO", value, configure, status);

