package de.tud.dvs.parser.operations.scopes;

import de.tud.dvs.parser.operations.Visitor;

public class JScopeSphere extends JScopeGeometricElement {

	/**
	 * 
	 */
	private int radius;

	/**
	 * 
	 */
	private JScopePoint center;

	/**
	 * 
	 */
	public JScopeSphere() {
	}

	/**
	 * 
	 * @param points
	 */
	public void setCenter(JScopePoint center) {
		if (center == null)
			throw new IllegalArgumentException(
					"The center point passed as parameter is null!");
		else
			this.center = center;
	}

	/**
	 * 
	 * @return
	 */
	public JScopePoint getCenter() {
		return this.center;
	}

	/**
	 * 
	 * @param radius
	 */
	public void setRadius(int radius) {
		if (radius < 0)
			throw new IllegalArgumentException(
					"The radius for this sphere is negative!");
		else
			this.radius = radius;
	}

	/**
	 * 
	 * @return
	 */
	public int getRadius() {
		return this.radius;
	}

	/**
	 * 
	 */
	public void accept(Visitor visitor) {
		visitor.visit(this);
	}

	/**
	 * 
	 */
	public String toString() {
		String r = "SPHERE (";

		r += center;

		r += ")";

		return r;
	}

}
