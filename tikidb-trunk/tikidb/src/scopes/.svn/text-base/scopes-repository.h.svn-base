#ifndef __SCOPES_REPOSITORY_H__
#define __SCOPES_REPOSITORY_H__

#include "contiki.h"

#define REPOSITORY_SIZE 10
#define REPOSITORY_TIMER_DURATION 10

PROCESS_NAME(repository_process);

enum repository_fields { SENSOR_LIGHT_PAR_RAW,
                         SENSOR_LIGHT_TSR_RAW,
                         SENSOR_TEMPERATURE_RAW,
                         SENSOR_TEMPERATURE_CELSIUS,
                         SENSOR_TEMPERATURE_FAHRENHEIT,
                         SENSOR_HUMIDITY_RAW,
                         SENSOR_HUMIDITY_PERCENT,
                         SENSOR_VOLTAGE_RAW,
                         NODE_ID
};

int scopes_repository_value(int index);

#endif
