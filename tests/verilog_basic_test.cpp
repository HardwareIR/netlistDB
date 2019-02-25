
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE netlistDB_to_hdl_friendly_test

#include <boost/test/unit_test.hpp>
#include <functional>
#include <iostream>
#include <fstream>
#include <stdint.h>
#include <sstream>

#include <netlistDB/netlist.h>
#include <netlistDB/statement_if.h>
#include <netlistDB/hw_type/common.h>
#include <netlistDB/transform/to_hdl_friendly.h>
#include <netlistDB/serializer/verilog.h>

using namespace std;
using namespace netlistDB;
using namespace netlistDB::hw_type;
using namespace netlistDB::transform;
using namespace netlistDB::serializer;



BOOST_AUTO_TEST_SUITE( netlistDB_to_hdl_friendly_testsuite )

BOOST_AUTO_TEST_CASE( simple_adder ) {
	Netlist ctx("adder");
	auto &a = ctx.sig_in("a", hw_int32);
	auto &b = ctx.sig_in("b", hw_int32);
	auto &c = ctx.sig_out("c", hw_int32);
	c(a + b);

	TransformToHdlFriendly t;
	t.apply(ctx);

	stringstream str;
	Verilog2001 ser;
	ser.serialize(ctx, str);

}





//____________________________________________________________________________//

BOOST_AUTO_TEST_SUITE_END()
