#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE TransformRemoveUseless_test

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
#include <tbb/tbb.h>

#include <netlistDB/transform/remove_useless.h>
#include "test_graphs.h"
#include "../src/debug_utils/timer.h"

using namespace netlistDB;
using namespace netlistDB::transform;

BOOST_AUTO_TEST_SUITE( TransformRemoveUseless_testsuite )

BOOST_AUTO_TEST_CASE( simple_traversal_100 ) {
	size_t N = 100;
	for (size_t thread_cnt = 2; thread_cnt <= 14; thread_cnt+=12) {
		tbb::task_scheduler_init init(thread_cnt);
		std::vector<iNode*> outputs;
		Netlist ctx("example");
		std::mt19937 rand(0);
		build_random_circuit(ctx, N, N, N, N, rand);

		TransformRemoveUseless t;
		BOOST_CHECK_EQUAL(ctx.nodes.size(), 20090);

		auto ti = new Timer(std::string("size ") + std::to_string(N) + " threads:" + std::to_string(thread_cnt));
		bool rm = t.apply(ctx);
		delete ti;
		size_t after_cleanup_cnt = 2822;
		size_t nets_after_cleanup_cnt = 1461;

		BOOST_CHECK_EQUAL(rm, true);
		BOOST_CHECK_EQUAL(ctx.nodes.size(), after_cleanup_cnt);
		BOOST_CHECK_EQUAL(ctx.nets.size(), nets_after_cleanup_cnt);

		ti = new Timer(std::string("clean size ") + std::to_string(N) + " threads:" + std::to_string(thread_cnt));
		rm = t.apply(ctx);
		delete ti;

		BOOST_CHECK_EQUAL(rm, false);
		BOOST_CHECK_EQUAL(ctx.nodes.size(), after_cleanup_cnt);
		BOOST_CHECK_EQUAL(ctx.nets.size(), nets_after_cleanup_cnt);
	}
}
//____________________________________________________________________________//

BOOST_AUTO_TEST_SUITE_END()
