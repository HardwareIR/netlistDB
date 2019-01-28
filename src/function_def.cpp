#include "netlist.h"

using namespace netlistDB;

FunctionDef::FunctionDef(const std::string & name, size_t arg_cnt) :
		name(name), arg_cnt(arg_cnt) {
}

Signal & FunctionDef::apply(Signal & arg0) {
	auto & res = arg0.ctx.sig();
	FunctionCall node(*this, arg0, res);
	return res;
}

Signal & FunctionDef::apply(Signal & arg0, Signal & arg1) {
	auto & res = arg0.ctx.sig();
	FunctionCall node(*this, arg0, arg1, res);
	return res;
}
