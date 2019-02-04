
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE netlistDB_large_query_test

#include <boost/test/unit_test.hpp>
#include <functional>
#include <iostream>
#include <fstream>
#include <stdint.h>
#include <random>

#include "../src/netlist.h"
#include "../src/statemen_if.h"
#include "../src/query/query_match.h"
#include "../src/operator_defs.h"


using namespace netlistDB;
using namespace netlistDB::query;

BOOST_AUTO_TEST_SUITE( netlistDB_testsuite )

template<typename T>
T select_any(std::vector<T> & objs, std::mt19937 & rand) {
	std::uniform_int_distribution<size_t> uniform_dist(0, objs.size() - 1);
	return objs.at(uniform_dist(rand));
}

std::map<FunctionDef*, std::function<Net&(Net&)>> apply_op_1 = {
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


std::map<FunctionDef*, std::function<Net&(Net&, Net&)>> apply_op_2 = {
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

std::vector<FunctionDef*> ops = { &OpNeq, &OpAnd, &OpOr, &OpXor, &OpLE,
		&OpGE, &OpGt, &OpLt, &OpEq, &OpUnMinus, &OpAdd,
		&OpSub, &OpDiv, &OpMul, &OpConcat, &OpRising, &OpFalling, };

Net & apply_op(FunctionDef* op, Net& arg0) {
	auto fn = apply_op_1.find(op);
	if (fn == apply_op_1.end())
		throw std::runtime_error("invalid unary operator " + op->name);
	return fn->second(arg0);
}

Net & apply_op(FunctionDef* op, Net& arg0, Net& arg1) {
	auto fn = apply_op_2.find(op);
	if (fn == apply_op_2.end())
		throw std::runtime_error("invalid binary operator " + op->name);
	return fn->second(arg0, arg1);
}

void build_random_circuit(Netlist & ctx, size_t input_cnt, size_t height,
		size_t connection_per_level, std::mt19937 & rand) {
	std::vector<Net*> nets;
	for (size_t i = 0; i < input_cnt; i++) {
		auto name = std::string("in") + std::to_string(i);
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
				throw std::runtime_error("NotImplemented");
			}
			nets.push_back(new_net);
		}
	}
}

BOOST_AUTO_TEST_CASE( query_add ) {
	Netlist ctx("test");
	std::mt19937 rand(0);
	build_random_circuit(ctx, 10, 10, 10, rand);

	QueryMatch query_add;
	query_add.sig("a") + query_add.sig("b");
	auto qres = query_add.search(ctx);

	BOOST_CHECK_EQUAL(qres.size(), 3);
}

BOOST_AUTO_TEST_CASE( query_mac ) {
	Netlist ctx("test");
	std::mt19937 rand(0);
	build_random_circuit(ctx, 100, 100, 100, rand);

	QueryMatch query_mac;
	query_mac.sig("a") + (query_mac.sig("b") * query_mac.sig("c") );
	auto qres = query_mac.search(ctx);

	BOOST_CHECK_EQUAL(qres.size(), 5);
}



//____________________________________________________________________________//

BOOST_AUTO_TEST_SUITE_END()
