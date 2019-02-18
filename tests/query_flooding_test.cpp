
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE netlistDB_test

#include <boost/test/unit_test.hpp>
#include <functional>
#include <iostream>
#include <fstream>
#include <stdint.h>

#include <netlistDB/netlist.h>
#include <netlistDB/statement_if.h>
#include <netlistDB/hw_type/common.h>

using namespace netlistDB;
using namespace netlistDB::hw_type;

BOOST_AUTO_TEST_SUITE( netlistDB_testsuite )

BOOST_AUTO_TEST_CASE( simple_adder ) {
	Netlist ctx("adder");
	auto &a = ctx.sig_in("a", hw_int32);
	auto &b = ctx.sig_in("b", hw_int32);
	auto &c = ctx.sig_out("c", hw_int32);
	auto &ab0 = a + b;
	auto &ab1 = a + b;
	BOOST_CHECK_EQUAL(&ab0, &ab1);
	c(a + b);
}

BOOST_AUTO_TEST_CASE( simple_mux ) {
	Netlist ctx("mux");
	auto &a = ctx.sig_in("a", hw_bit);
	auto &b = ctx.sig_in("b", hw_bit);
	auto &c = ctx.sig_in("c", hw_bit);

	If(a)({
		&a(b),
	}).Else({
		&a(c),
	});

}



//____________________________________________________________________________//

BOOST_AUTO_TEST_SUITE_END()
