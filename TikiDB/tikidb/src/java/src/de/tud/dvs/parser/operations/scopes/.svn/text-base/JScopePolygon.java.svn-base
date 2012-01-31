package de.tud.dvs.parser.operations.scopes;

import java.util.Vector;

import de.tud.dvs.parser.operations.Visitor;

public class JScopePolygon extends JScopeGeometricElement {

	/**
	 * 
	 */
	private Vector<JScopePoint> points;

	/**
	 * 
	 */
	public JScopePolygon() {
	}

	/**
	 * 
	 * @param points
	 */
	public void setPoints(Vector<JScopePoint> points) {
		if (points == null)
			throw new IllegalArgumentException("The points passed as parameter are null!");
		else if (points.size() < 3)
			throw new IllegalArgumentException("The number of points for this polygon is less than 3!");
		else if (points.size() > 255)
			throw new IllegalArgumentException("The number of points for this polygon is less than 3!");
		else
			this.points = points;
	}

	/**
	 * 
	 * @return
	 */
	public Vector<JScopePoint> getPoints() {
		return this.points;
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
		String r = "POLYGON (";
		
		for (JScopePoint p : points) {
			r += p;
		}
		
		r += ")";
		
		return r;
	}
	
}
