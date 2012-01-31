package de.tud.dvs.parser.operations.scopes;

import java.util.Vector;

import de.tud.dvs.parser.operations.Visitor;

public class JScopeGeometricOperation extends JScopeTerm {

	// comparison operators
	public static final int IN_POLYGON = 20;
	public static final int IN_SPHERE = 21;
	public static final int IN_SEGMENT = 22;

	private int geometricOperator;

	private Vector<JScopeGeometricElement> elements;

	public JScopeGeometricOperation() {
		this.elements = new Vector<JScopeGeometricElement>();
	}
	
	public void setGeometricOperator(int geometricOperator)
			throws IllegalArgumentException {
		if ((geometricOperator >= 20) && (geometricOperator <= 22))
			this.geometricOperator = geometricOperator;
		else
			throw new IllegalArgumentException(
					"The specified geometric operator is invalid!");
	}
	
	public void addGeometricElement(JScopeGeometricElement element) {
		if (element != null)
			this.elements.add(element);
		else
			throw new IllegalArgumentException("The element passed as parameter is null");
	}
	
	public int getGeometricOperator() {
		return this.geometricOperator;
	}
	
	public Vector<JScopeGeometricElement> getGeometricElements() {
		return this.elements;
	}

	public void accept(Visitor visitor) {
		visitor.visit(this);
	}
}
