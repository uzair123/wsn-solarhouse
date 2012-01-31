package de.tud.dvs.parser.operations.scopes;


public abstract class JScopeLogicalExpression extends JScopeExpression {

	// logical operators
	public final static int AND = 10;
	public final static int OR = 11;
	public final static int NOT = 12;

	private int operator;

	public void setOperator(int operator) throws IllegalArgumentException {
		if ((operator >= 10) && (operator <= 12))
			this.operator = operator;
		else
			throw new IllegalArgumentException(
					"The specified logical operator is invalid!");
	}

	public int getOperator() {
		return this.operator;
	}
	
	public String toString() {
		switch (this.operator) {
		case AND:	return "&";
		case OR:	return "|";
		case NOT:	return "~";
		default: return " ? ";
		}
	}


}
