#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE netlistDB_statement_if_test

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

BOOST_AUTO_TEST_SUITE (netlistDB_statement_if_testsuite)

BOOST_AUTO_TEST_CASE( object_props ) {
	{
		Netlist ctx("test");
		auto &a = ctx.sig_in("a", hw_int32);
		auto &b = ctx.sig_out("b", hw_int32);
		auto &c = ctx.sig_in("c", hw_int32);
		auto assig = dynamic_cast<Assignment*>(&b(c));
		IfStatement &ifs = If(a)( { assig, });

		BOOST_CHECK_EQUAL(ifs.parent, nullptr);
		BOOST_CHECK_EQUAL(assig->parent, &ifs);
		BOOST_CHECK_EQUAL(ifs.ifTrue_specified, true);
		BOOST_CHECK_EQUAL(ifs.ifFalse_specified, false);
		BOOST_CHECK_EQUAL(ifs.elseIf.size(), 0);

		BOOST_TEST(ifs.ifTrue == std::vector<Statement*>({assig}),
				tt::per_element());

		// check properties of the internal assignment
		BOOST_CHECK_EQUAL(&assig->dst, &b);
		BOOST_CHECK_EQUAL(&assig->src, &c);
		BOOST_CHECK_EQUAL(assig->dst_index.size(), 0);

		std::vector<Net*> inp_vec( { &a, &c });
		std::vector<Net*> c_vec( { &c });
		std::vector<Net*> outp_vec( { &b });

		BOOST_TEST(assig->_outputs == outp_vec, tt::per_element());
		BOOST_TEST(assig->_inputs == c_vec, tt::per_element());

		BOOST_TEST(ifs._outputs == outp_vec, tt::per_element());
		BOOST_TEST(ifs._inputs == inp_vec, tt::per_element());

	}

}

//____________________________________________________________________________//

BOOST_AUTO_TEST_SUITE_END()
