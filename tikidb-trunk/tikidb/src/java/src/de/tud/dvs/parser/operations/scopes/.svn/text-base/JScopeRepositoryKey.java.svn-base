package de.tud.dvs.parser.operations.scopes;

import de.tud.dvs.parser.operations.Visitor;

public class JScopeRepositoryKey extends JScopeOperand {

	public final static int REPOSITORY_KEY = 133;

	private int repoKey;

	public void setRepositoryKey(int repoKey) throws IllegalArgumentException {
		if ((repoKey > 0) && (repoKey < 256))
			this.repoKey = repoKey;
		else
			throw new IllegalArgumentException(
					"The specified REPOSITORY KEY id is invalid!");

	}

	public int getRepositoryKey() {
		return this.repoKey;
	}

	public void accept(Visitor visitor) {
		visitor.visit(this);
	}
}
