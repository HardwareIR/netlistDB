#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE netlistDB_statement_assignment_test

#include <boost/test/unit_test.hpp>
#include <functional>
#include <iostream>
#include <fstream>
#include <stdint.h>

#include "../src/netlist.h"
#include "../src/statement_if.h"
#include "../src/statement_assignment.h"
#include "../src/hw_type/common.h"

namespace tt = boost::test_tools;
using namespace netlistDB;
using namespace netlistDB::hw_type;

BOOST_AUTO_TEST_SUITE (netlistDB_statement_assignment_testsuite)

BOOST_AUTO_TEST_CASE( object_props ) {
	Netlist ctx("adder");
	{
		auto &a = ctx.sig_in("a", hw_int32);
		auto &b = ctx.sig_out("b", hw_int32);
		auto assig = dynamic_cast<Assignment *>(&a(b));
		BOOST_CHECK_EQUAL(assig->parent, nullptr);
		BOOST_CHECK_EQUAL(&assig->dst, &a);
		BOOST_CHECK_EQUAL(&assig->src, &b);
		BOOST_CHECK_EQUAL(assig->dst_index.size(), 0);

		std::vector<Net*> a_vec({&a});
		std::vector<Net*> b_vec({&b});
		BOOST_TEST(assig->_outputs == a_vec, tt::per_element());
		BOOST_TEST(assig->_inputs == b_vec, tt::per_element());
	}
	{
		auto &a = ctx.sig_in("a", hw_int32);
		auto &i = ctx.sig_in("i", hw_int32);
		auto &b = ctx.sig_out("b", hw_int32);
		auto assig = dynamic_cast<Assignment *>(&a[i](b));
		BOOST_CHECK_EQUAL(assig->parent, nullptr);
		BOOST_CHECK_EQUAL(&assig->dst, &a);

		std::vector<iNode*> assig_vec({assig});
		BOOST_TEST(a.drivers == assig_vec, tt::per_element());
        BOOST_CHECK_EQUAL(a.endpoints.size(), 1); // slice op
        BOOST_CHECK_EQUAL(i.drivers.size(), 0);
        BOOST_CHECK_EQUAL(b.drivers.size(), 0);
        BOOST_TEST(i.endpoints == std::vector<iNode*>({a[i].drivers[0], assig}), tt::per_element());
        BOOST_TEST(b.endpoints == assig_vec, tt::per_element());

		BOOST_CHECK_EQUAL(&assig->dst, &a);
		BOOST_CHECK_EQUAL(&assig->src, &b);
		BOOST_CHECK_EQUAL(assig->dst_index.size(), 1);
		BOOST_CHECK_EQUAL(assig->dst_index[0], &i);

		std::vector<Net*> a_vec({&a});
		std::vector<Net*> ib_vec({&i, &b});
		BOOST_TEST(assig->_outputs == a_vec, tt::per_element());
		BOOST_TEST(assig->_inputs == ib_vec, tt::per_element());
	}
}

//____________________________________________________________________________//

BOOST_AUTO_TEST_SUITE_END()
