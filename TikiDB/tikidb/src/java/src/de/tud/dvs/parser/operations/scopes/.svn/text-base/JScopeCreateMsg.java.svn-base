package de.tud.dvs.parser.operations.scopes;

import de.tud.dvs.parser.operations.Visitor;



public class JScopeCreateMsg extends JScopeMsg {

	JScopeExpression expression;
	
	int superScopeId;
	
	public void setJScopeExpression(JScopeExpression exp) {
		this.expression = exp;
	}
	
	public JScopeExpression getJScopeExpression() {
		return this.expression;
		
	}
	
	public void setSuperScopeId(int superScopeId) throws IllegalArgumentException {
		if ((superScopeId < 0) || (superScopeId > 65535))
			throw new IllegalArgumentException("The super scope id is not valid (not in the range [0..65535])");
		this.superScopeId = superScopeId;
	}
	
	public int getSuperScopeId() {
		return this.superScopeId;		
	}
	
	public void accept(Visitor visitor){
		visitor.visit(this);
	}
}
