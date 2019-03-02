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
#include <netlistDB/serializer/verilog.h>
#include <netlistDB/transform/to_hdl_friendly.h>

using namespace std;
using namespace netlistDB;
using namespace netlistDB::hw_type;
using namespace netlistDB::serializer;
using namespace netlistDB::transform;


BOOST_AUTO_TEST_SUITE( netlistDB_to_hdl_friendly_testsuite )

BOOST_AUTO_TEST_CASE( simple_wire_module ) {
	Netlist ctx("wire_module");
	auto &a_in = ctx.sig_in("a_in", hw_int32);
	auto &a_out = ctx.sig_out("a_out", hw_int32);
	a_out(a_in);

	TransformToHdlFriendly t;
	t.apply(ctx);

	Verilog2001 ser;
	{
		stringstream str;
		//ser.serialize(ctx, str);
		//std::cerr << str.str() << std::endl;
		//BOOST_CHECK_EQUAL(str.str(), "[TODO]");
	}
}


//____________________________________________________________________________//

BOOST_AUTO_TEST_SUITE_END()
