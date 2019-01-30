#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE netlistDB_basic_query_test

#include <boost/test/unit_test.hpp>
#include <functional>
#include <iostream>
#include <fstream>
#include <stdint.h>

#include "../src/netlist.h"
#include "../src/statemen_if.h"
#include "../src/query/query.h"

using namespace netlistDB;
using namespace netlistDB::query;

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
		auto & c = ctx.sig_in("c");
		~a;
		res & c;

		auto path = Query::find_path(a, res);
		BOOST_CHECK_EQUAL(path.second, true);
		BOOST_CHECK_EQUAL(path.first.size(), 3);
		if (path.first.size() == 3) {
			std::vector<iNode*> ref = {&a, res.drivers[0].fnCall, &res};
			size_t i = 0;
			for (auto p: path.first) {
				BOOST_CHECK_EQUAL(p, ref[i]);
				i++;
			}
		}
	}


	Query query_add;
	query_add.sig("a") + query_add.sig("b");


	auto qres = query_add.search(ctx);
	BOOST_CHECK_EQUAL(qres.size(), n);

}

//____________________________________________________________________________//

BOOST_AUTO_TEST_SUITE_END()
