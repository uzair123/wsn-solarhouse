package de.tud.dvs.parser.operations;

import java.util.Vector;

import de.tud.dvs.parser.operations.scopes.JScopeBinaryLogicalExpression;
import de.tud.dvs.parser.operations.scopes.JScopeComparison;
import de.tud.dvs.parser.operations.scopes.JScopeConstant;
import de.tud.dvs.parser.operations.scopes.JScopeCreateMsg;
import de.tud.dvs.parser.operations.scopes.JScopeDataMsg;
import de.tud.dvs.parser.operations.scopes.JScopeExistsSensor;
import de.tud.dvs.parser.operations.scopes.JScopeExpression;
import de.tud.dvs.parser.operations.scopes.JScopeGeometricOperation;
import de.tud.dvs.parser.operations.scopes.JScopeMsg;
import de.tud.dvs.parser.operations.scopes.JScopeNodeId;
import de.tud.dvs.parser.operations.scopes.JScopeNodePosition;
import de.tud.dvs.parser.operations.scopes.JScopeOperand;
import de.tud.dvs.parser.operations.scopes.JScopePoint;
import de.tud.dvs.parser.operations.scopes.JScopePolygon;
import de.tud.dvs.parser.operations.scopes.JScopeRemoveMsg;
import de.tud.dvs.parser.operations.scopes.JScopeRepositoryKey;
import de.tud.dvs.parser.operations.scopes.JScopeSphere;
import de.tud.dvs.parser.operations.scopes.JScopeUnaryLogicalExpression;
import de.tud.dvs.scopes.util.Tools;

public class SOSSerializationVisitor implements Visitor {

	// Operations on scopes: (from scope_uart.c :: enum scope_operation_t)
	public final static int OPERATION_SCOPE_CREATE = 0;
	public final static int OPERATION_SCOPE_SENDDATA = 1;
	public final static int OPERATION_SCOPE_REMOVE = 2;

	// Empty scope (from scope_properties.h)
	public final static int NOOP = 0;

	// comparison operators
	public static int EQUAL = 1;
	public static int DISTINCT = 2;
	public static int GREATERTHAN = 3;
	public static int LESSTHAN = 4;
	public static int GREATERTHANOREQUAL = 5;
	public static int LESSTHANOREQUAL = 6;

	// Node abilities
	public final static int EXISTS_SENSOR = 30;

	// logical operators
	public final static int AND = 10;
	public final static int OR = 11;
	public final static int NOT = 12;

	// types of values
	public final static int VAL_BOOL = 128;
	public final static int VAL_UI8 = 129;
	public final static int VAL_I8 = 130;
	public final static int VAL_UI16 = 131;
	public final static int VAL_I16 = 132;
	public final static int REPOSITORY_KEY = 133;
	public final static int NODE_ID = 134;
	public final static int NODE_POSITION = 135;

	// coordinate values (sequence of 2 or 3 unsigned, 16-bit integers)
	public final static int VAL_COORD2D = 140;
	public final static int VAL_COORD3D = 141;

	public final static int VAL_POLYGON = 145;
	public final static int VAL_SPHERE = 146;

	// SOS message types
	public final static int MSG_SCOPE_UART = 47;

	private Vector<Byte> output;

	public SOSSerializationVisitor() {
		output = new Vector<Byte>();
	}

	public void visit(JScopeMsg msg) {
		msg.accept(this);
	}

	public void visit(JScopeCreateMsg msg) {
		if (output.size() > 0)
			output.clear();

		// Destination Module (scope_uart, whose PID == 204)
		// 1 byte
		output.add(Tools.sign(204));

		// Source Module
		// 1 byte
		output.add(Tools.sign(0));

		// Destination Address (Broadcast == 65535)
		// 2 bytes
		int destinationAddress = 65535;
		output.add(Tools.sign(destinationAddress % 256));
		output.add(Tools.sign(destinationAddress / 256));

		// Source Address (0)
		// 2 bytes
		int sourceAddress = 0;
		output.add(Tools.sign(sourceAddress % 256));
		output.add(Tools.sign(sourceAddress / 256));

		// Message Type
		// 1 byte
		int type = MSG_SCOPE_UART;
		output.add(Tools.sign(type));

		// SOS's "scope_uart" payload Length
		// 1 byte
		output.add(Tools.sign(0));

		// SuperScope Id
		// 2 bytes
		int superScopeId = msg.getSuperScopeId();
		output.add(Tools.sign(superScopeId % 256));
		output.add(Tools.sign(superScopeId / 256));

		// Scope Id
		// 2 bytes
		int scopeId = msg.getScopeId();
		output.add(Tools.sign(scopeId % 256));
		output.add(Tools.sign(scopeId / 256));

		// Operation type: 0: add, 1: delete, 2: query
		int operation = OPERATION_SCOPE_CREATE;
		output.add(Tools.sign(operation));

		// SOS's "scope" payload Length
		// 1 byte
		output.add(Tools.sign(0));

		// Now, dive recursively
		JScopeExpression exp = msg.getJScopeExpression();
		exp.accept(this);

		// Lastly, the payload lengths must be modified
		// First, for scope_uart
		byte scope_uart_msg_length = (byte) (output.size() - 8);
		output.removeElementAt(7);
		output.insertElementAt(Tools.sign(scope_uart_msg_length), 7);

		// Second, for scope
		byte scope_msg_length = (byte) (output.size() - 14);
		output.removeElementAt(13);
		output.insertElementAt(Tools.sign(scope_msg_length), 13);
	}

	public void visit(JScopeDataMsg msg) {
		if (output.size() > 0)
			output.clear();

		// Destination Module (scope_uart, whose PID == 204)
		// 1 byte
		output.add(Tools.sign(204));

		// Source Module
		// 1 byte
		output.add(Tools.sign(0));

		// Destination Address (Broadcast == 65535)
		// 2 bytes
		int destinationAddress = 65535;
		output.add(Tools.sign(destinationAddress % 256));
		output.add(Tools.sign(destinationAddress / 256));

		// Source Address (0)
		// 2 bytes
		int sourceAddress = 0;
		output.add(Tools.sign(sourceAddress % 256));
		output.add(Tools.sign(sourceAddress / 256));

		// Message Type
		// 1 byte
		int type = MSG_SCOPE_UART;
		output.add(Tools.sign(type));

		// SOS's "scope_uart" payload Length
		// 1 byte
		output.add(Tools.sign(0));

		// SuperScope Id
		// 2 bytes
		short superScopeId = 0;
		output.add(Tools.sign(superScopeId % 256));
		output.add(Tools.sign(superScopeId / 256));

		// Scope Id
		// 2 bytes
		int scopeId = msg.getScopeId();
		output.add(Tools.sign(scopeId % 256));
		output.add(Tools.sign(scopeId / 256));

		// Operation type: 0: add, 1: delete, 2: query
		int operation = OPERATION_SCOPE_SENDDATA;
		output.add(Tools.sign(operation));

		// SOS's "scope" payload Length
		// 1 byte
		output.add(Tools.sign(0));

		// SOS's destination module id
		// 1 byte
		output.add(Tools.sign(msg.getDestinationModuleId()));

		// SOS's destination module id
		// 1 byte
		output.add(Tools.sign(msg.getSourceModuleId()));

		// Message Type
		// 1 byte
		output.add(Tools.sign(msg.getMessageType()));

		// Now, insert data payload
		for (int payloadInt : msg.getPayload())
			output.add(Tools.sign(payloadInt));

		// Lastly, the payload lengths must be modified
		// First, for scope_uart
		byte scope_uart_msg_length = (byte) (output.size() - 8);
		output.removeElementAt(7);
		output.insertElementAt(Tools.sign(scope_uart_msg_length), 7);

		// Second, for SOS's "scope" payload Length
		byte scope_msg_length = (byte) (output.size() - 14);
		output.removeElementAt(13);
		output.insertElementAt(Tools.sign(scope_msg_length), 13);

	}

	public void visit(JScopeRemoveMsg rm) {
		if (output.size() > 0)
			output.clear();

		// Destination Module (scope_uart, whose PID == 204)
		// 1 byte
		output.add(Tools.sign(204));

		// Source Module ( is ignored --> 0 )
		// 1 byte
		output.add(Tools.sign(0));

		// Destination Address (Broadcast --> 65535)
		// 2 bytes
		int destinationAddress = 65535;
		output.add(Tools.sign(destinationAddress % 256));
		output.add(Tools.sign(destinationAddress / 256));

		// Source Address ( is ignored --> 0 )
		// 2 bytes
		int sourceAddress = 0;
		output.add(Tools.sign(sourceAddress % 256));
		output.add(Tools.sign(sourceAddress / 256));

		// Message Type
		// 1 byte
		int type = MSG_SCOPE_UART;
		output.add(Tools.sign(type));

		// Payload Length (in this case we know it is = 5)
		// 1 byte
		output.add(Tools.sign(5));

		// SuperScope Id
		// This field is ignored!!
		// 2 bytes
		short superScopeId = 0;
		output.add(Tools.sign(superScopeId % 256));
		output.add(Tools.sign(superScopeId / 256));

		// Scope Id
		// 2 bytes
		int scopeId = rm.getScopeId();
		output.add(Tools.sign(scopeId % 256));
		output.add(Tools.sign(scopeId / 256));

		// Operation type: 0: add, 1: delete, 2: query
		int operation = OPERATION_SCOPE_REMOVE;
		output.add(Tools.sign(operation));
	}

	public void visit(JScopeBinaryLogicalExpression blex) {
		switch (blex.getOperator()) {
		case AND: {
			output.add(Tools.sign(AND));
			break;
		}
		case OR: {
			output.add(Tools.sign(OR));
			break;
		}
		default: {
			throw new RuntimeException("The unary operator '"
					+ blex.getOperator() + "' is not recognized!");
		}
		}

		JScopeExpression exp1 = blex.getOperand1();
		JScopeExpression exp2 = blex.getOperand2();
		exp1.accept(this);
		exp2.accept(this);
	}

	public void visit(JScopeUnaryLogicalExpression ulex) {
		switch (ulex.getOperator()) {
		case NOT: {
			output.add(Tools.sign(NOT));
			break;
		}
		default: {
			throw new RuntimeException("The unary operator '"
					+ ulex.getOperator() + "' is not recognized!");
		}
		}

		JScopeExpression exp = ulex.getOperand();
		exp.accept(this);
	}

	public void visit(JScopeNodeId nid) {
		output.add(Tools.sign(NODE_ID));
	}

	public void visit(JScopeNodePosition pos) {
		output.add(Tools.sign(NODE_POSITION));
	}

	/**
	 * Depending on the value, serialize this data type:
	 * 
	 * <pre>
	 * VAL_BOOL:  [     0 ..     1 ]
	 * VAL_UI8:   [     0 ..   255 ]
	 * VAL_I8:    [  -128 ..   127 ]
	 * VAL_UI16:  [     0 .. 65535 ]
	 * VAL_I16:   [-32768 .. 32767 ]
	 * </pre>
	 */
	public void visit(JScopeConstant ct) {
		if (ct.isBoolean()) {
			output.add(Tools.sign(VAL_BOOL));
			if (ct.getBooleanValue())
				output.add(Tools.sign(1));
			else
				output.add(Tools.sign(0));
		} else {
			int value = ct.getNumericValue();

			// is it an uint8_t? [0..255]
			if ((value >= 0) && (value <= 255)) {
				output.add(Tools.sign(VAL_UI8));
				output.add(Tools.sign(value));
			}
			// is it an int8_t? [-128..127]
			else if ((value >= -128) && (value <= 127)) {
				output.add(Tools.sign(VAL_I8));

				if (value < 0)
					value = 256 + value;

				output.add(Tools.sign(value));
			}
			// is it an uint16_t? [0..65535]
			else if ((value >= 0) && (value <= 65535)) {
				output.add(Tools.sign(VAL_UI16));
				output.add(Tools.sign(value % 256));
				output.add(Tools.sign(value / 256));
			}
			// is it an int16_t? [-32768..32767]
			else if ((value >= -32768) && (value <= 32767)) {
				output.add(Tools.sign(VAL_I16));

				if (value < 0)
					value = 65536 + value;

				output.add(Tools.sign(value % 256));
				output.add(Tools.sign(value / 256));

			}
		}
	}

	public void visit(JScopeRepositoryKey key) {
		output.add(Tools.sign(REPOSITORY_KEY));
		output.add(Tools.sign(key.getRepositoryKey()));
	}

	public void visit(JScopeComparison cmp) {
		output.add(Tools.sign(cmp.getComparisonOperator()));
		JScopeOperand op1 = cmp.getOperand1(), op2 = cmp.getOperand2();
		op1.accept(this);
		op2.accept(this);
	}

	public void visit(JScopeExistsSensor es) {
		// Code for EXISTS_SENSOR
		// 1 byte
		output.add(Tools.sign(EXISTS_SENSOR));

		// Code for the Sensor referenced
		// 1 byte
		int sensor = es.getSensor();
		output.add(Tools.sign(sensor));

	}

	/**
	 * 
	 */
	public void visit(JScopeGeometricOperation oper) {
		// @TODO implement
		int gOper = oper.getGeometricOperator();

		output.add(Tools.sign(gOper));

		switch (gOper) {
		case JScopeGeometricOperation.IN_POLYGON: {
			for (Visitable v : oper.getGeometricElements()) {
				v.accept(this);
			}

			break;
		}
		case JScopeGeometricOperation.IN_SPHERE: {
			for (Visitable v : oper.getGeometricElements()) {
				v.accept(this);
			}

			break;
		}
		default: {
			throw new RuntimeException("The geometrical operator '" + gOper
					+ "' is not implemented!");
		}
		}

	}

	/**
	 * 
	 */
	public void visit(JScopePoint point) {
		int tmp;
		output.add(Tools.sign(VAL_COORD3D));

		// X coordinate
		tmp = point.getx();
		if (tmp < 0)
			tmp = 65536 + tmp;
		output.add(Tools.sign(tmp % 256));
		output.add(Tools.sign(tmp / 256));

		// Y coordinate
		tmp = point.gety();
		if (tmp < 0)
			tmp = 65536 + tmp;
		output.add(Tools.sign(tmp % 256));
		output.add(Tools.sign(tmp / 256));

		// Z coordinate
		tmp = point.getz();
		if (tmp < 0)
			tmp = 65536 + tmp;
		output.add(Tools.sign(tmp % 256));
		output.add(Tools.sign(tmp / 256));
	}

	/**
	 * 
	 */
	public void visit(JScopePolygon poly) {
		int tmp;
		output.add(Tools.sign(VAL_POLYGON));

		output.add(Tools.sign(poly.getPoints().size()));

		for (JScopePoint point : poly.getPoints()) {

			// X coordinate
			tmp = point.getx();
			if (tmp < 0)
				tmp = 65536 + tmp;
			output.add(Tools.sign(tmp % 256));
			output.add(Tools.sign(tmp / 256));

			// Y coordinate
			tmp = point.gety();
			if (tmp < 0)
				tmp = 65536 + tmp;
			output.add(Tools.sign(tmp % 256));
			output.add(Tools.sign(tmp / 256));

			// Z coordinate
			tmp = point.getz();
			if (tmp < 0)
				tmp = 65536 + tmp;
			output.add(Tools.sign(tmp % 256));
			output.add(Tools.sign(tmp / 256));
		}
	}

	/**
	 * 
	 */
	public void visit(JScopeSphere sphere) {
		int tmp;
		output.add(Tools.sign(VAL_SPHERE));

		int radius = sphere.getRadius();
		output.add(Tools.sign(radius % 256));
		output.add(Tools.sign(radius / 256));

		JScopePoint center = sphere.getCenter();

		// X coordinate
		tmp = center.getx();
		if (tmp < 0)
			tmp = 65536 + tmp;
		output.add(Tools.sign(tmp % 256));
		output.add(Tools.sign(tmp / 256));

		// Y coordinate
		tmp = center.gety();
		if (tmp < 0)
			tmp = 65536 + tmp;
		output.add(Tools.sign(tmp % 256));
		output.add(Tools.sign(tmp / 256));

		// Z coordinate
		tmp = center.getz();
		if (tmp < 0)
			tmp = 65536 + tmp;
		output.add(Tools.sign(tmp % 256));
		output.add(Tools.sign(tmp / 256));
		
	}

	/**
	 * 
	 * @return
	 */
	public byte[] getOutput() {
		byte[] result = null;
		if (output != null) {
			result = new byte[output.size()];
			for (int i = 0; i < output.size(); i++)
				result[i] = ((Byte) output.elementAt(i)).byteValue();
		}
		return result;
	}
}
