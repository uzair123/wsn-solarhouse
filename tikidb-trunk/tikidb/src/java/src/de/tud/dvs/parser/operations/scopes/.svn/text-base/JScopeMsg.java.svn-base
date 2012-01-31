package de.tud.dvs.parser.operations.scopes;

import de.tud.dvs.parser.operations.Visitable;

public abstract class JScopeMsg implements Visitable {

	private int scopeId;

	public void setScopeId(int scopeId) throws IllegalArgumentException {
		if ((scopeId < 0) || (scopeId > 65535))
			throw new IllegalArgumentException("The scope id is not valid (not in the range [0..65535])");
		this.scopeId = scopeId;
	}
	
	public int getScopeId() {
		return this.scopeId;
	}
}
