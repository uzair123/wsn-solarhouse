package de.tud.dvs.parser.operations;

import java.io.InputStream;
import java.io.OutputStream;

public interface Streamable {

	public void write(OutputStream out);
	
	public void read(InputStream in);
}
