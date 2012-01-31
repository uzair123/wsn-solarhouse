package de.tud.dvs.parser.operations.scopes;

import de.tud.dvs.parser.operations.Visitor;


public class JScopeBinaryLogicalExpression extends JScopeLogicalExpression {

	private JScopeExpression operand1;
	private JScopeExpression operand2;

	public void setOperand1(JScopeExpression operand)
			throws IllegalArgumentException {
		if (operand != null)
			this.operand1 = operand;
		else
			throw new IllegalArgumentException("The specified operand is null");
	}

	public JScopeExpression getOperand1() {
		return this.operand1;
	}

	public void setOperand2(JScopeExpression operand)
			throws IllegalArgumentException {
		if (operand != null)
			this.operand2 = operand;
		else
			throw new IllegalArgumentException("The specified operand is null");
	}

	public JScopeExpression getOperand2() {
		return this.operand2;
	}

	public String toString() {
		return "(" + operand1.toString() + " " + super.toString() + " "
				+ operand2.toString() + ")";
	}

	public void accept(Visitor visitor) {
		visitor.visit(this);
	}
}