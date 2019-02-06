
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
#include "../src/statement_if.h"
#include "../src/query/query_match.h"
#include "../src/operator_defs.h"
#include "../src/query/query_traverse.h"
#include "test_graphs.h"

using namespace netlistDB;
using namespace netlistDB::query;

BOOST_AUTO_TEST_SUITE( netlistDB_testsuite )

std::vector<FunctionCall*> find_ops(Netlist & ctx, FunctionDef & op) {
	// traverse and count the number of the add operators
	QueryTraverse q(ctx.nodes.size());
	std::vector<iNode*> inputs;
	for (auto n : ctx.nets) {
		if (n->direction == Direction::DIR_IN) {
			inputs.push_back(n);
		}
	}

	std::vector<FunctionCall*> ops;
	auto cb = [&ops, &op] (iNode& n) {
		auto _n = dynamic_cast<FunctionCall*>(&n);
		if (_n and &_n->fn == &op) {
			ops.push_back(_n);
		}
		return QueryTraverse::dummy_callback(n);
	};
	q.traverse(inputs, cb, 1);
	return ops;
}

BOOST_AUTO_TEST_CASE( query_add ) {
	Netlist ctx("test");
	std::mt19937 rand(0);
	build_random_circuit(ctx, 10, 10, 10, 10, rand);

	QueryMatch query_add;
	auto &r = query_add.sig_in("a") + query_add.sig_in("b");
	r.direction = Direction::DIR_OUT;

	auto qres = query_add.search(ctx);
	BOOST_CHECK_EQUAL(qres.size(), 29);

	auto adds = find_ops(ctx, OpAdd);
	BOOST_CHECK_EQUAL(qres.size(), 29);

}

BOOST_AUTO_TEST_CASE( query_mac ) {
	Netlist ctx("test");
	std::mt19937 rand(0);
	build_random_circuit(ctx, 100, 100, 100, 100, rand);

	QueryMatch query_mac;
	auto &r = query_mac.sig_in("a")
			+ (query_mac.sig_in("b") * query_mac.sig_in("c"));
	r.direction = Direction::DIR_OUT;
	auto qres = query_mac.search(ctx);

	BOOST_CHECK_EQUAL(qres.size(), 479);
}

//____________________________________________________________________________//

BOOST_AUTO_TEST_SUITE_END()
