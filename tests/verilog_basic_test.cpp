
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
#include <netlistDB/transform/to_hdl_friendly.h>

using namespace netlistDB;
using namespace netlistDB::hw_type;
using namespace netlistDB::transform;


BOOST_AUTO_TEST_SUITE( netlistDB_testsuite )

BOOST_AUTO_TEST_CASE( simple_adder ) {
	Netlist ctx("adder");
	auto &a = ctx.sig_in("a", hw_int32);
	auto &b = ctx.sig_in("b", hw_int32);
	auto &c = ctx.sig_out("c", hw_int32);
	c(a + b);

	TransformToHdlFriendly t;
	t.apply(ctx);

}





//____________________________________________________________________________//

BOOST_AUTO_TEST_SUITE_END()
