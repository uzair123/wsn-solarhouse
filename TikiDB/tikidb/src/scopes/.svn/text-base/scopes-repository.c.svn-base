#include "contiki.h"
#include "dev/sht11-sensor.h"
#include "dev/light-sensor.h"
#include "dev/battery-sensor.h"
#include "node-id.h"

#include "scopes-repository.h"
#include "scopes-types.h"

typedef void (*repository_func)(void);

static int repository[REPOSITORY_SIZE];
static repository_func collect_functions[];

char *repository_fieldnames[] = { "light par (raw)", "light tsr (raw)",
		"temp (raw)", "temp (c)", "temp (f)", "hum (raw)", "hum (%)",
		"voltage (raw)", "node id" };

PROCESS(repository_process, "repository process")
;

static void sensor_light_par_raw(void) {
	//  repository[SENSOR_LIGHT_PAR_RAW] = sensors_light1();
	repository[SENSOR_LIGHT_PAR_RAW] = light_sensor.value(LIGHT_SENSOR_PHOTOSYNTHETIC);
}

static void sensor_light_tsr_raw(void) {
	//  repository[SENSOR_LIGHT_TSR_RAW] = sensors_light2();
	repository[SENSOR_LIGHT_TSR_RAW] = light_sensor.value(LIGHT_SENSOR_TOTAL_SOLAR);
}

static void sensor_temperature_raw(void) {
	repository[SENSOR_TEMPERATURE_RAW] = sht11_sensor.value(SHT11_SENSOR_TEMP);
}

static void sensor_temperature_celsius(void) {
	repository[SENSOR_TEMPERATURE_CELSIUS] = -39.60 + 0.01 * sht11_sensor.value(SHT11_SENSOR_TEMP);
}

static void sensor_temperature_fahrenheit(void) {
	repository[SENSOR_TEMPERATURE_FAHRENHEIT] = (-39.60 + 0.01 * sht11_sensor.value(SHT11_SENSOR_TEMP))
			* 9 / 5 + 32;
}

static void sensor_humidity_raw(void) {
	repository[SENSOR_HUMIDITY_RAW] = sht11_sensor.value(SHT11_SENSOR_HUMIDITY);
}

static void sensor_humidity_percent(void) {
	int h = sht11_sensor.value(SHT11_SENSOR_HUMIDITY);
	repository[SENSOR_HUMIDITY_PERCENT] = -4 + 0.0405 * h - 2.8e-6 * (h * h);
}

static void sensor_voltage_raw(void) {
	repository[SENSOR_VOLTAGE_RAW] = battery_sensor.value(0);
}

static void init(void) {
	repository[NODE_ID] = node_id;
}

static repository_func collect_functions[] = { sensor_light_par_raw,
		sensor_light_tsr_raw, sensor_temperature_raw,
		sensor_temperature_celsius, sensor_temperature_fahrenheit,
		sensor_humidity_raw, sensor_humidity_percent, sensor_voltage_raw };

int scopes_repository_value(int index) {
	if (index >= 0 && index < REPOSITORY_SIZE) {
		return repository[index];
	}
	return -1;
}

/* repository process */
PROCESS_THREAD(repository_process, ev, data) {
	PROCESS_BEGIN();

		init();

		PRINTF(3,"repository process started\n");

		/* create and start an event timer */
		static struct etimer repository_timer;
		etimer_set(&repository_timer, REPOSITORY_TIMER_DURATION * CLOCK_SECOND);

		do {
			/* update the repository */
			int i;
			for (i = 0; i < sizeof(collect_functions)/sizeof(repository_func); i++) {
				collect_functions[i]();
				PRINTF(3,"%s: %d\n", repository_fieldnames[i], repository[i]);
			}

			PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&repository_timer));
			etimer_reset(&repository_timer);
		}while(1);

		PROCESS_END();
	}
