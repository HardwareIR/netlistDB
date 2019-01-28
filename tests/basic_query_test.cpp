#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE netlistDB_test

#include <boost/test/unit_test.hpp>
#include <functional>
#include <iostream>
#include <fstream>
#include <stdint.h>

#include "../src/netlist.h"
#include "../src/statemen_if.h"
#include "../src/query.h"

using namespace netlistDB;

BOOST_AUTO_TEST_SUITE( netlistDB_testsuite )

BOOST_AUTO_TEST_CASE( query_result_of_add ) {
	Netlist ctx("adder");
	std::vector<std::tuple<Net *, Net *, Net *>> expected;
	size_t n = 20;

	for (size_t i = 0; i < n; i++) {
		auto &a = ctx.sig_in("a");
		auto &b = ctx.sig_in("b");
		auto &res = a + b;
		expected.push_back( { &a, &b, &res });

		// add some garbage
		ctx.sig_in("c");
		~a;
	}

	Query query_add;
	query_add.sig() + query_add.sig();

	auto qres = query_add.search(ctx);
	BOOST_CHECK_EQUAL(qres.size(), n);

}

//____________________________________________________________________________//

BOOST_AUTO_TEST_SUITE_END()
