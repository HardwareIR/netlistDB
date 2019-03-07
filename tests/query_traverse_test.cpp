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

#include <netlistDB/query/query_traverse.h>
#include "../src/debug_utils/timer.h"
#include "test_graphs.h"

using namespace std;
using namespace netlistDB;
using namespace netlistDB::query;

BOOST_AUTO_TEST_SUITE( netlistDB_testsuite )

// [fixme] thread_cnt not not set properly
void tf_test(vector<iNode*> & outputs, QueryTraverse& q, size_t obj_cnt,
		size_t thread_cnt) {

	atomic<size_t> visited_cnt(0);
	auto callback = [&visited_cnt](iNode& n, const std::function<void(iNode &)> & select) {
		//usleep(1);
		visited_cnt++;
		QueryTraverse::dummy_callback(n, select);
	};
	q.clean_visit_flags();
	//q.load_balance_limit = 1;
	auto t = new Timer(string("threads ") + to_string(thread_cnt));
	q.traverse(outputs, callback);
	delete t;
	BOOST_CHECK_EQUAL(visited_cnt, obj_cnt);
}

Netlist* build_graph(vector<iNode*> & outputs, size_t N,
		size_t expected_node_cnt) {
	auto ctx = new Netlist("example_circuit");
	mt19937 rand(0);

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
	vector<iNode*> outputs;
	Netlist * ctx = build_graph(outputs, N, expected_node_cnt);
	cout << "size " << N << " nodes:" << ctx->nodes.size() << endl;

	QueryTraverse q(ctx->nodes.size());
	//for (size_t i = 1; i <= thread::hardware_concurrency(); i++) {
	for (size_t i = 1; i <= 2; i++) {
		tf_test(outputs, q, expected_node_cnt, i);
	}
}

//BOOST_AUTO_TEST_CASE( simple_traversal_500 ) {
//	size_t N = 500;
//	size_t expected_node_cnt = 496620;
//	vector<iNode*> outputs;
//	Netlist * ctx = build_graph(outputs, N, expected_node_cnt);
//	cout << "size " << N << " nodes:" << ctx->nodes.size() << endl;
//
//	QueryTraverse q(ctx->nodes.size());
//	for (size_t i = 1; i <= thread::hardware_concurrency(); i++) {
//		tf_test(outputs, q, expected_node_cnt, i);
//	}
//}
//
//BOOST_AUTO_TEST_CASE( simple_traversal_750 ) {
//	size_t N = 750;
//	size_t expected_node_cnt = 1115936;
//	vector<iNode*> outputs;
//	Netlist * ctx = build_graph(outputs, N, expected_node_cnt);
//	cout << "size " << N << " nodes:" << ctx->nodes.size() << endl;
//
//	QueryTraverse q(ctx->nodes.size());
//	for (size_t i = 1; i <= thread::hardware_concurrency(); i++) {
//		tf_test(outputs, q, expected_node_cnt, i);
//	}
//}
//
//BOOST_AUTO_TEST_CASE( simple_traversal_1000 ) {
//	size_t N = 1000;
//	size_t expected_node_cnt = 1983146;
//	vector<iNode*> outputs;
//	Netlist * ctx = build_graph(outputs, N, expected_node_cnt);
//	cout << "size " << N << " nodes:" << ctx->nodes.size() << endl;
//
//	QueryTraverse q(ctx->nodes.size());
//	for (size_t i = 1; i <= thread::hardware_concurrency(); i++) {
//		tf_test(outputs, q, expected_node_cnt, i);
//	}
//}
//
//BOOST_AUTO_TEST_CASE( simple_traversal_2000 ) {
//	size_t N = 2000;
//	size_t expected_node_cnt = 7926124;
//	vector<iNode*> outputs;
//	Netlist * ctx = build_graph(outputs, N, expected_node_cnt);
//	cout << "size " << N << " nodes:" << ctx->nodes.size() << endl;
//
//	QueryTraverse q(ctx->nodes.size());
//	for (size_t i = 1; i <= thread::hardware_concurrency(); i++) {
//		tf_test(outputs, q, expected_node_cnt, i);
//	}
//}
//____________________________________________________________________________//

BOOST_AUTO_TEST_SUITE_END()
