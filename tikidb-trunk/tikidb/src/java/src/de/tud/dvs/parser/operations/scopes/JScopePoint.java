package de.tud.dvs.parser.operations.scopes;

import de.tud.dvs.parser.operations.Visitor;



public class JScopePoint extends JScopeGeometricElement {

	/**
	 * 
	 */
	private int x;
	
	/**
	 * 
	 */
	private int y;
	
	/**
	 * 
	 */
	private int z;

	/**
	 * 
	 * @param x
	 * @throws IllegalArgumentException
	 */
	public void setx(int x) throws IllegalArgumentException {
		if ((x >= -32768 ) && (x <= 32767))
			this.x = x;
		else
			throw new IllegalArgumentException("The specified x value is out of range!");
	}

	/**
	 * 
	 * @param y
	 * @throws IllegalArgumentException
	 */
	public void sety(int y) throws IllegalArgumentException {
		if ((y >= -32768 ) && (y <= 32767))
			this.y = y;
		else
			throw new IllegalArgumentException("The specified y value is out of range!");
	}

	/**
	 * 
	 * @param z
	 * @throws IllegalArgumentException
	 */
	public void setz(int z) throws IllegalArgumentException {
		if ((z >= -32768 ) && (z <= 32767))
			this.z = z;
		else
			throw new IllegalArgumentException("The specified z value is out of range!");
	}

	/**
	 * 
	 * @return
	 */
	public int getx() {
		return this.x;
	}

	/**
	 * 
	 * @return
	 */
	public int gety() {
		return this.y;
	}
	
	/**
	 * 
	 * @return
	 */
	public int getz() {
		return this.z;
	}
	
	/**
	 * 
	 */
	public String toString() {
		return "[" + x + "," + y + "," + z + "]";
	}
	
	/**
	 * 
	 */
	public void accept(Visitor visitor){
		visitor.visit(this);
	}

}
