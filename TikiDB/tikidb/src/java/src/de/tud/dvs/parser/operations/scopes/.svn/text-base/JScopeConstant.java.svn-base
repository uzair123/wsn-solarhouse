package de.tud.dvs.parser.operations.scopes;

import de.tud.dvs.parser.operations.Visitor;



public class JScopeConstant extends JScopeOperand {

	private boolean isBoolean;
	private boolean booleanValue;
	private int numericValue;
	
	public boolean isBoolean() {
		return this.isBoolean;
	}
	
	public void setNumericValue(int value) throws IllegalArgumentException {
		this.isBoolean = false;
		if ((value >= -32768) && (value < 65536))
			this.numericValue = value;
		else
			throw new IllegalArgumentException("The specified value, " + value + "  is out of range!");
	}

	public void setBooleanValue(boolean value) {
		this.isBoolean = true;
		this.booleanValue = value;
	}
	
	public int getNumericValue() {
		return this.numericValue;
	}
	
	public boolean getBooleanValue() {
		return this.booleanValue;
	}
	
	public void accept(Visitor visitor){
		visitor.visit(this);
	}

}
