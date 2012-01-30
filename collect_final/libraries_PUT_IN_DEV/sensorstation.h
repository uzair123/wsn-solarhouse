#ifndef __SENSORSTATION_H__
#define __SENSORSTATION_H__
#include <stdio.h>

struct sensor_values 
{
	int16_t co2;
	int16_t humidity;
	int16_t temperature;
} _sensor_values;

void  sensorstation_init(void);

struct sensor_values sensorstation_read();

/* -------------------------------------------------------------------------- */
#endif /* ifndef __SENSORSTATION_H__ */
