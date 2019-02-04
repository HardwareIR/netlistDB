#include "netlist.h"

using namespace netlistDB;

FunctionCall::FunctionCall(size_t index, FunctionDef & fn, Net & arg0, Net & res) :
		OperationNode(index), fn(fn), args( { &arg0 }), res(res) {
	arg0.endpoints.push_back(this);
	res.drivers.push_back(this);

}

FunctionCall::FunctionCall(size_t index, FunctionDef & fn, Net & arg0, Net & arg1, Net & res) :
		OperationNode(index), fn(fn), args( { &arg0, &arg1 }), res(res) {
	arg0.endpoints.push_back(this);
	arg1.endpoints.push_back(this);
	res.drivers.push_back(this);
}

iNode::iterator FunctionCall::forward() {
	iNode::iterator it;
	it.push_back(&res);
	return it;
}

iNode::iterator FunctionCall::backward() {
	iNode::iterator it;
	for (auto i : args)
		it.push_back(i);
	return it;
}
