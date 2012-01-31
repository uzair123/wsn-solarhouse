package de.tud.dvs.parser.operations.scopes;

import de.tud.dvs.parser.operations.Visitor;

public class JScopeUnaryLogicalExpression extends JScopeLogicalExpression {

	private JScopeExpression operand;

	public void setOperand(JScopeExpression operand)
			throws IllegalArgumentException {
		if (operand != null)
			this.operand = operand;
		else
			throw new IllegalArgumentException("The specified operand is null");
	}

	public JScopeExpression getOperand() {
		return this.operand;
	}

	public String toString() {
		return super.toString() + " ( " + operand.toString() + " )";
	}

	public void accept(Visitor visitor) {
		visitor.visit(this);
	}
}
