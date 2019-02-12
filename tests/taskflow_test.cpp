#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE netlistDB_test

#include <boost/test/unit_test.hpp>
#include <functional>
#include <iostream>
#include <fstream>
#include <stdint.h>
#include <random>
#include <chrono>
#include <cstring>
#include <atomic>
#include <boost/range/join.hpp>

#include "test_graphs.h"
#include "../src/query/query_traverse.h"
#include "timer.h"

using namespace netlistDB;
using namespace netlistDB::query;


BOOST_AUTO_TEST_SUITE( netlistDB_testsuite )

void tf_test(std::vector<iNode*> & outputs, QueryTraverse& q, size_t obj_cnt,
		size_t thread_cnt) {
	std::atomic<size_t> visited_cnt = 0;
	auto callback = [&visited_cnt](iNode & n) {
			//usleep(1);
			visited_cnt++;
			return QueryTraverse::dummy_callback(n);
		};
	q.clean_visit_flags(thread_cnt);
	auto t = new Timer(std::string("threads ") + std::to_string(thread_cnt));
	q.traverse(outputs, callback, thread_cnt);
	delete t;
	BOOST_CHECK_EQUAL(visited_cnt, obj_cnt);
}

Netlist* build_graph(std::vector<iNode*> & outputs, size_t N,
		size_t expected_node_cnt) {
	auto ctx = new Netlist("example_circuit");
	std::mt19937 rand(0);

	auto t = new Timer("construction time");
	build_random_circuit(*ctx, N, N, N, N, rand);
	delete t;
	BOOST_CHECK_EQUAL(ctx->nodes.size(), expected_node_cnt);

	for (auto & n : ctx->nets) {
		if (n->direction == Direction::DIR_OUT) {
			outputs.push_back(n);
		}
	}
	return ctx;
}

BOOST_AUTO_TEST_CASE( simple_traversal_100 ) {
	size_t N = 100;
	size_t expected_node_cnt = 20090;
	std::vector<iNode*> outputs;
	Netlist * ctx = build_graph(outputs, N, expected_node_cnt);

	QueryTraverse q(ctx->nodes.size());
	//q.load_balance_limit = 1;
	for (size_t i = 1; i <= std::thread::hardware_concurrency(); i++) {
		tf_test(outputs, q, expected_node_cnt, i);
	}
}

//BOOST_AUTO_TEST_CASE( simple_traversal_500 ) {
//	size_t N = 500;
//	size_t expected_node_cnt = 1983146;
//	std::vector<iNode*> outputs;
//	Netlist * ctx = build_graph(outputs, N, expected_node_cnt);
//
//	QueryTraverse q(ctx->nodes.size());
//	//q.load_balance_limit = 1;
//	for (size_t i = 1; i <= std::thread::hardware_concurrency(); i++) {
//		tf_test(outputs, q, expected_node_cnt, i);
//	}
//}
//
//BOOST_AUTO_TEST_CASE( simple_traversal_750 ) {
//	size_t N = 750;
//	size_t expected_node_cnt = 1983146;
//	std::vector<iNode*> outputs;
//	Netlist * ctx = build_graph(outputs, N, expected_node_cnt);
//
//	QueryTraverse q(ctx->nodes.size());
//	//q.load_balance_limit = 1;
//	for (size_t i = 1; i <= std::thread::hardware_concurrency(); i++) {
//		tf_test(outputs, q, expected_node_cnt, i);
//	}
//}
//
//BOOST_AUTO_TEST_CASE( simple_traversal_1000 ) {
//	size_t N = 1000;
//	size_t expected_node_cnt = 1983146;
//	std::vector<iNode*> outputs;
//	Netlist * ctx = build_graph(outputs, N, expected_node_cnt);
//
//	QueryTraverse q(ctx->nodes.size());
//	//q.load_balance_limit = 1;
//	for (size_t i = 1; i <= std::thread::hardware_concurrency(); i++) {
//		tf_test(outputs, q, expected_node_cnt, i);
//	}
//}

//BOOST_AUTO_TEST_CASE( simple_traversal_2000 ) {
//	size_t N = 2000;
//	size_t expected_node_cnt = 1983146;
//	std::vector<iNode*> outputs;
//	Netlist * ctx = build_graph(outputs, N, expected_node_cnt);
//
//	for (size_t i = 8; i <= std::thread::hardware_concurrency(); i++) {
//		tf_test(*ctx, outputs, i);
//	}
//}
//____________________________________________________________________________//

BOOST_AUTO_TEST_SUITE_END()
