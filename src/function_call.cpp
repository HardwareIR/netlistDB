#include <netlistDB/netlist.h>

namespace netlistDB {

void init_iterators(FunctionCall * fncall) {
	fncall->forward.push_back(&fncall->res);
	fncall->backward.push_back(reinterpret_cast<std::vector<iNode*>*>(&fncall->args));
}

FunctionCall::FunctionCall(FunctionDef & fn, Net & arg0, Net & res) :
		fn(fn), args( { &arg0 }), res(res) {
	arg0.ctx.register_node(*this);
	arg0.endpoints.push_back(this);
	res.drivers.push_back(this);
	init_iterators(this);
}

FunctionCall::FunctionCall(FunctionDef & fn, Net & arg0, Net & arg1, Net & res) :
		fn(fn), args( { &arg0, &arg1 }), res(res) {
	arg0.ctx.register_node(*this);
	arg0.endpoints.push_back(this);
	arg1.endpoints.push_back(this);
	res.drivers.push_back(this);
	init_iterators(this);
}

}
