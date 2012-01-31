package de.tud.dvs.parser.operations.scopes;

import java.util.Vector;

import de.tud.dvs.parser.operations.Visitor;


public class JScopeDataMsg extends JScopeMsg {

	private int destinationModuleId;
	private int sourceModuleId;
	private int messageType;
	
	private Vector<Integer> payload;
	
	public JScopeDataMsg() {
		this.payload = new Vector<Integer>(); 
	}
	
	public String toString() {
		return "( [" + payload.size() + "] )";
	}
	
	public void addPayloadInteger(int i) {
		this.payload.add(i);
	}

	public Vector<Integer> getPayload() {
		return this.payload;
	}

	public void accept(Visitor visitor) {
		visitor.visit(this);
	}
	
	public void setDestinationModuleId(int destinationModuleId) {
		this.destinationModuleId = destinationModuleId;
	}
	
	public void setSourceModuleId(int sourceModuleId) {
		this.sourceModuleId = sourceModuleId;
	}
	
	public void setMessageType(int messageType) {
		this.messageType = messageType;
	}
	
	public int getDestinationModuleId() {
		return this.destinationModuleId;
	}
	
	public int getSourceModuleId() {
		return this.sourceModuleId;
	}
	
	public int getMessageType() {
		return this.messageType;
	}
}