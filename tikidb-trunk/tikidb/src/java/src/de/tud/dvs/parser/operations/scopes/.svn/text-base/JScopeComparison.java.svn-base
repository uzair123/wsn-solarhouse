package de.tud.dvs.parser.operations.scopes;

import de.tud.dvs.parser.operations.Visitor;


public class JScopeComparison extends JScopeTerm {

	// comparison operators
	public static int EQUAL = 1;
	public static int DISTINCT = 2;
	public static int GREATERTHAN = 3;
	public static int LESSTHAN = 4;
	public static int GREATERTHANOREQUAL = 5;
	public static int LESSTHANOREQUAL = 6;

	private int comparisonOperator;

	private JScopeOperand operand1;
	private JScopeOperand operand2;

	public void setOperand1(JScopeOperand operand)
			throws IllegalArgumentException {
		if (operand != null)
			this.operand1 = operand;
		else
			throw new IllegalArgumentException("The specified operand is null");
	}

	public JScopeOperand getOperand1() {
		return this.operand1;
	}

	public void setOperand2(JScopeOperand operand)
			throws IllegalArgumentException {
		if (operand != null)
			this.operand2 = operand;
		else
			throw new IllegalArgumentException("The specified operand is null");
	}

	public JScopeOperand getOperand2() {
		return this.operand2;
	}

	public void setComparisonOperator(int comparisonOperator)
			throws IllegalArgumentException {
		if ((comparisonOperator >= 1) && (comparisonOperator <= 6))
			this.comparisonOperator = comparisonOperator;
		else
			throw new IllegalArgumentException(
					"The specified comparison operator is invalid!");
	}

	public int getComparisonOperator() {
		return this.comparisonOperator;
	}

	public void accept(Visitor visitor) {
		visitor.visit(this);
	}
}
