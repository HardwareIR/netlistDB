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

#include "test_graphs.h"
#include "../src/transform/remove_useless.h"

using namespace netlistDB;
using namespace netlistDB::transform;


BOOST_AUTO_TEST_SUITE( TransformRemoveUseless_testsuite )


BOOST_AUTO_TEST_CASE( simple_traversal_100 ) {
	size_t N = 100;
	std::vector<iNode*> outputs;
	Netlist ctx("example");
	std::mt19937 rand(0);
	build_random_circuit(ctx, N, N, N, N, rand);

	TransformRemoveUseless t;
	BOOST_CHECK_EQUAL(ctx.nodes.size(), 20090);

	t.apply(ctx);
	size_t after_cleanup_cnt = 20090;

	BOOST_CHECK_EQUAL(ctx.nodes.size(), after_cleanup_cnt);

	t.apply(ctx);
	BOOST_CHECK_EQUAL(ctx.nodes.size(), after_cleanup_cnt);
}
//____________________________________________________________________________//

BOOST_AUTO_TEST_SUITE_END()
