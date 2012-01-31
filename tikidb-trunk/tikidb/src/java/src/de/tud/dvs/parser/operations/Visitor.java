package de.tud.dvs.parser.operations;

import de.tud.dvs.parser.operations.scopes.JScopeBinaryLogicalExpression;
import de.tud.dvs.parser.operations.scopes.JScopeComparison;
import de.tud.dvs.parser.operations.scopes.JScopeConstant;
import de.tud.dvs.parser.operations.scopes.JScopeCreateMsg;
import de.tud.dvs.parser.operations.scopes.JScopeDataMsg;
import de.tud.dvs.parser.operations.scopes.JScopeExistsSensor;
import de.tud.dvs.parser.operations.scopes.JScopeGeometricOperation;
import de.tud.dvs.parser.operations.scopes.JScopeMsg;
import de.tud.dvs.parser.operations.scopes.JScopeNodeId;
import de.tud.dvs.parser.operations.scopes.JScopeNodePosition;
import de.tud.dvs.parser.operations.scopes.JScopePoint;
import de.tud.dvs.parser.operations.scopes.JScopePolygon;
import de.tud.dvs.parser.operations.scopes.JScopeRemoveMsg;
import de.tud.dvs.parser.operations.scopes.JScopeRepositoryKey;
import de.tud.dvs.parser.operations.scopes.JScopeSphere;
import de.tud.dvs.parser.operations.scopes.JScopeUnaryLogicalExpression;


public interface Visitor {
	void visit(JScopeMsg msg);

	void visit(JScopeCreateMsg msg);
	void visit(JScopeDataMsg msg);
	void visit(JScopeRemoveMsg msg);

	void visit(JScopeBinaryLogicalExpression blex);
	void visit(JScopeUnaryLogicalExpression ulex);

	void visit(JScopeComparison cmp);
	void visit(JScopeExistsSensor es);

	void visit(JScopeNodeId nid);
	void visit(JScopeConstant ct);
	void visit(JScopeRepositoryKey key);
	
	void visit(JScopeGeometricOperation oper);
	void visit(JScopePoint pos);
	void visit(JScopePolygon poly);
	void visit(JScopeSphere sphere);
	void visit(JScopeNodePosition pos);
}
