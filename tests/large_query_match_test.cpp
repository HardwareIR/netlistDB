
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
#include "test_graphs.h"

using namespace netlistDB;
using namespace netlistDB::query;

BOOST_AUTO_TEST_SUITE( netlistDB_testsuite )


BOOST_AUTO_TEST_CASE( query_add ) {
	Netlist ctx("test");
	std::mt19937 rand(0);
	build_random_circuit(ctx, 10, 10, 10, 10, rand);

	QueryMatch query_add;
	query_add.sig("a") + query_add.sig("b");
	auto qres = query_add.search(ctx);

	BOOST_CHECK_EQUAL(qres.size(), 3);
}

BOOST_AUTO_TEST_CASE( query_mac ) {
	Netlist ctx("test");
	std::mt19937 rand(0);
	build_random_circuit(ctx, 100, 100, 100, 100, rand);

	QueryMatch query_mac;
	query_mac.sig("a") + (query_mac.sig("b") * query_mac.sig("c") );
	auto qres = query_mac.search(ctx);

	BOOST_CHECK_EQUAL(qres.size(), 5);
}



//____________________________________________________________________________//

BOOST_AUTO_TEST_SUITE_END()
