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
		stringstream ref;
		ref << "module wire_module(" << endl;
		ref << "    input a_in," << endl;
		ref << "    output a_out);" << endl;
		ref << endl;
		ref << "    assign a_out = a_in;" << endl;
		ref << endl;
		ref << "endmodule";
		ser.serialize(ctx, str);
		BOOST_CHECK_EQUAL(str.str(), ref.str());
	}
}

//BOOST_AUTO_TEST_CASE( simple_ff_module ) {
//	Netlist ctx("ff_module");
//
//	auto &clk = ctx.sig_in("clk", hw_bit);
//	auto &a_in = ctx.sig_in("a_in", hw_int32);
//	auto &a_out = ctx.sig_out("a_out", hw_int32);
//	If(clk.rising()) (
//			&a_out(a_in)
//	);
//	a_out(a_in);
//
//	TransformToHdlFriendly t;
//	t.apply(ctx);
//
//	Verilog2001 ser;
//	{
//		stringstream str;
//		stringstream ref;
//		ser.serialize(ctx, str);
//		cerr << str.str() << endl;
//		BOOST_CHECK_EQUAL(str.str(), ref.str());
//	}
//}

//____________________________________________________________________________//

BOOST_AUTO_TEST_SUITE_END()
