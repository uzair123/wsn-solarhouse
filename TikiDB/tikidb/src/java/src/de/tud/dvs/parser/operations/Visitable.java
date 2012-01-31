package de.tud.dvs.parser.operations;

public interface Visitable {
	public void accept(Visitor visitor);
}
