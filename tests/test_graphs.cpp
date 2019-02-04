#include "test_graphs.h"

using namespace std;
using namespace netlistDB;

template<typename T>
T select_any(vector<T> & objs, mt19937 & rand) {
	uniform_int_distribution<size_t> uniform_dist(0, objs.size() - 1);
	return objs.at(uniform_dist(rand));
}

map<FunctionDef*, function<Net&(Net&)>> apply_op_1 = {
	{&OpNeg, [](Net& arg0) -> Net& {
		return ~arg0;
	}},
	{&OpUnMinus, [](Net& arg0) -> Net& {
		return -arg0;
	}},
	{&OpRising, [](Net& arg0) -> Net&  {
		return arg0.rising();
	}},
	{&OpFalling, [](Net& arg0) -> Net&  {
		return arg0.falling();
	}},
};


map<FunctionDef*, function<Net&(Net&, Net&)>> apply_op_2 = {
	{&OpAnd, [](Net& arg0, Net& arg1) -> Net&  {
		return arg0 & arg1;
	}},
	{&OpOr, [](Net& arg0, Net& arg1) -> Net& {
		return arg0 | arg1;
	}},
	{&OpXor, [](Net& arg0, Net& arg1) -> Net& {
		return arg0 ^ arg1;
	}},
	{&OpLE, [](Net& arg0, Net& arg1) -> Net& {
		return arg0 <= arg1;
	}},
	{&OpGE, [](Net& arg0, Net& arg1) -> Net& {
		return arg0 >= arg1;
	}},
	{&OpGt, [](Net& arg0, Net& arg1) -> Net& {
		return arg0 > arg1;
	}},
	{&OpLt, [](Net& arg0, Net& arg1) -> Net& {
		return arg0 < arg1;
	}},
	{&OpEq, [](Net& arg0, Net& arg1) -> Net& {
		return arg0 == arg1;
	}},
	{&OpNeq, [](Net& arg0, Net& arg1) -> Net& {
		return arg0 != arg1;
	}},
	{&OpAdd, [](Net& arg0, Net& arg1) -> Net& {
		return arg0 + arg1;
	}},
	{&OpSub, [](Net& arg0, Net& arg1) -> Net& {
		return arg0 - arg1;
	}},
	{&OpDiv, [](Net& arg0, Net& arg1) -> Net& {
		return arg0 / arg1;
	}},
	{&OpMul, [](Net& arg0, Net& arg1) -> Net& {
		return arg0 * arg1;
	}},
	{&OpConcat, [](Net& arg0, Net& arg1) -> Net& {
		return arg0.concat(arg1);
	}},
};

vector<FunctionDef*> ops = { &OpNeq, &OpAnd, &OpOr, &OpXor, &OpLE,
		&OpGE, &OpGt, &OpLt, &OpEq, &OpUnMinus, &OpAdd,
		&OpSub, &OpDiv, &OpMul, &OpConcat, &OpRising, &OpFalling, };

Net & apply_op(FunctionDef* op, Net& arg0) {
	auto fn = apply_op_1.find(op);
	if (fn == apply_op_1.end())
		throw runtime_error("invalid unary operator " + op->name);
	return fn->second(arg0);
}

Net & apply_op(FunctionDef* op, Net& arg0, Net& arg1) {
	auto fn = apply_op_2.find(op);
	if (fn == apply_op_2.end())
		throw runtime_error("invalid binary operator " + op->name);
	return fn->second(arg0, arg1);
}

void build_random_circuit(Netlist & ctx, size_t input_cnt, size_t output_cnt, size_t height,
		size_t connection_per_level, mt19937 & rand) {
	vector<Net*> nets;
	for (size_t i = 0; i < input_cnt; i++) {
		auto name = string("in") + to_string(i);
		nets.push_back(&(ctx.sig_in(name)));
	}

	for (size_t lvl = 0; lvl < height; lvl++) {
		for (size_t i = 0; i < height; i++) {
			auto op = select_any(ops, rand);
			Net * new_net = nullptr;
			switch (op->arg_cnt) {
			case 1:
				new_net = &apply_op(op, *select_any(nets, rand));
				break;
			case 2:
				new_net = &apply_op(op, *select_any(nets, rand), *select_any(nets, rand));
				break;
			default:
				throw runtime_error("NotImplemented");
			}
			nets.push_back(new_net);
		}
	}
	for (size_t i = 0; i < output_cnt; i++) {
		auto name = string("out") + to_string(i);
		auto & o = ctx.sig_out(name);
		auto rand_sig = select_any(nets, rand);
		o(*rand_sig);
	}
}
