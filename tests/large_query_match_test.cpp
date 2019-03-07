#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE netlistDB_large_query_test

#include <boost/test/unit_test.hpp>
#include <functional>
#include <iostream>
#include <fstream>
#include <stdint.h>
#include <random>

#include <netlistDB/netlist.h>
#include <netlistDB/statement_if.h>
#include <netlistDB/hw_type/common.h>
#include <netlistDB/query/query_match.h>
#include <netlistDB/operator_defs.h>
#include <netlistDB/query/query_traverse.h>
#include "../src/debug_utils/timer.h"
#include "test_graphs.h"

using namespace netlistDB;
using namespace netlistDB::query;
using namespace netlistDB::hw_type;

BOOST_AUTO_TEST_SUITE( netlistDB_testsuite )
auto & t = hw_int32;
std::vector<FunctionCall*> find_ops(Netlist & ctx, FunctionDef & op) {
	// traverse and count the number of the add operators
	QueryTraverse q(ctx.nodes.size());
	std::vector<iNode*> inputs;
	for (auto n : ctx.nets) {
		if (n->direction == Direction::DIR_IN) {
			inputs.push_back(n);
		}
	}

	boost::mutex mx;

	std::vector<FunctionCall*> ops;
	auto cb = [&ops, &op, &mx] (iNode& n,
			const std::function<void(iNode &)> & select) {
		auto _n = dynamic_cast<FunctionCall*>(&n);
		if (_n and &_n->fn == &op) {
			boost::mutex::scoped_lock lock(mx);
			ops.push_back(_n);
		}
		QueryTraverse::dummy_callback(n, select);
	};
	q.traverse(inputs, cb);

	return ops;
}

BOOST_AUTO_TEST_CASE( query_add ) {
	Netlist ctx("test");
	std::mt19937 rand(0);
	build_random_circuit(ctx, 10, 10, 10, 10, rand);

	QueryMatch query_add;

	auto &r = query_add.sig_in("a", t) + query_add.sig_in("b", t);
	r.direction = Direction::DIR_OUT;

	auto qres = query_add.search(ctx);
	BOOST_CHECK_EQUAL(qres.size(), 29);

	// search from different end
	std::reverse(ctx.nets.begin(), ctx.nets.end());
	qres = query_add.search(ctx);
	BOOST_CHECK_EQUAL(qres.size(), 29);

	auto adds = find_ops(ctx, OpAdd);
	BOOST_CHECK_EQUAL(qres.size(), 29);
}

BOOST_AUTO_TEST_CASE( query_mac ) {
	size_t N = 100;

	Netlist ctx("test");
	std::mt19937 rand(0);
	build_random_circuit(ctx, N, N, N, N, rand);

	QueryMatch query_mac;
	auto &r = query_mac.sig_in("a", t)
			+ (query_mac.sig_in("b", t) * query_mac.sig_in("c", t));
	r.direction = Direction::DIR_OUT;

	auto t = new Timer(std::string("query_mac size:") + std::to_string(N));
	auto qres = query_mac.search(ctx);
	delete t;

	BOOST_CHECK_EQUAL(qres.size(), 479);
}

//____________________________________________________________________________//

BOOST_AUTO_TEST_SUITE_END()
