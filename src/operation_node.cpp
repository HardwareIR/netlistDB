#include "netlist.h"

using namespace netlistDB;

OperationNode::OperationNode(FunctionCall * fnCall) :
		fnCall(fnCall), stm(nullptr) {
	assert(fnCall);
}

OperationNode::OperationNode(Statement * stm) :
		fnCall(nullptr), stm(stm) {
	assert(stm);
}

bool OperationNode::isStm() const {
	return bool(stm);
}
bool OperationNode::isFn() const {
	return bool(fnCall);
}
bool OperationNode::operator==(const OperationNode& other) const {
	if (isStm()) {
		return stm == other.stm;
	} else {
		return fnCall == other.fnCall;
	}
}

bool OperationNode::operator!=(const OperationNode& other) const {
	return not (*this == other);
}

size_t OperationNode::hash() const {
	if (isStm())
		return std::hash<FunctionCall *> { }(fnCall);
	else
		return std::hash<Statement *> { }(stm);
}
