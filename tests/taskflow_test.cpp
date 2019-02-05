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
	auto start = std::chrono::system_clock::now();
	q.traverse(outputs, callback, thread_cnt);

	auto end = std::chrono::system_clock::now();
	std::cout << "threads " << thread_cnt << " Tf runtime: "
			<< std::chrono::duration_cast<std::chrono::milliseconds>(
					end - start).count() << " ms" << std::endl;
	BOOST_CHECK_EQUAL(visited_cnt, obj_cnt);

}

Netlist* build_graph(std::vector<iNode*> & outputs, size_t N,
		size_t expected_node_cnt) {
	auto ctx = new Netlist("example_circuit");
	std::mt19937 rand(0);

	auto start = std::chrono::system_clock::now();
	build_random_circuit(*ctx, N, N, N, N, rand);
	BOOST_CHECK_EQUAL(ctx->obj_seq_num, expected_node_cnt);

	auto end = std::chrono::system_clock::now();
	std::cout << "construction time: "
			<< std::chrono::duration_cast<std::chrono::milliseconds>(
					end - start).count() << " ms" << std::endl;

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

	QueryTraverse q(ctx->obj_seq_num);
	for (size_t i = 1; i <= std::thread::hardware_concurrency(); i++) {
		tf_test(outputs, q, expected_node_cnt, i);
	}
}

//BOOST_AUTO_TEST_CASE( simple_traversal_1000 ) {
//	size_t N = 1000;
//	size_t expected_node_cnt = 1983146;
//	std::vector<iNode*> outputs;
//	Netlist * ctx = build_graph(outputs, N, expected_node_cnt);
//
//	QueryTraverse q(ctx->obj_seq_num);
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
