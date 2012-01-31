package de.tud.dvs.parser.operations.scopes;

import de.tud.dvs.parser.operations.Visitor;


public class JScopeExistsSensor extends JScopeTerm {

	// reserved_repository_keys
	public static int SENSOR_LIGHT_AMBIENT = 1;
	public static int SENSOR_LIGHT_PAR = 2;
	public static int SENSOR_HUMIDITY = 3;
	public static int SENSOR_TEMPERATURE = 4;
	public static int SENSOR_INTERNAL_TEMPERATURE = 5;
	public static int SENSOR_ACCEL_X = 6;
	public static int SENSOR_ACCEL_Y = 7;
	public static int SENSOR_INTERNAL_VOLTAGE = 8;

	private int sensor;

	public void setSensor(int sensor) throws IllegalArgumentException {
		if ((sensor >= 1) && (sensor <= SENSOR_INTERNAL_VOLTAGE))
			this.sensor = sensor;
		else
			throw new IllegalArgumentException(
					"The specified sensor is invalid!");
	}

	public int getSensor() {
		return this.sensor;
	}

	public void accept(Visitor visitor) {
		visitor.visit(this);
	}
}
