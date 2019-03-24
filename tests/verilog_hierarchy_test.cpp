#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE netlistDB_verilog_hierarchy_test

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

#include "components/wire.h"

using namespace std;
using namespace netlistDB;
using namespace netlistDB::hw_type;
using namespace netlistDB::serializer;
using namespace netlistDB::transform;

BOOST_AUTO_TEST_SUITE (netlistDB_verilog_hierarchy_testsuite)

BOOST_AUTO_TEST_CASE( simple_wire_module ) {
	Wire wire(hw_int32);

	Netlist ctx("wire_container_module");
	auto &a_in = ctx.sig_in("a_in", hw_int32);
	auto &a_out = ctx.sig_out("a_out", hw_int32);
	auto &b_in = ctx.sig_in("b_in", hw_int32);
	auto &b_out = ctx.sig_out("b_out", hw_int32);

	auto &w1 = ctx.add_component(wire);
	w1.connect(a_in, wire.a_in);
	w1.connect(a_out, wire.a_out);

	b_out(b_in);

	TransformToHdlFriendly t;
	t.apply(ctx);

	{
		stringstream str;
		Verilog2001 ser(str);
		stringstream ref;
		ref << "module wire_module(" << endl;
		ref << "    input signed [32-1:0] a_in," << endl;
		ref << "    output signed [32-1:0] a_out);" << endl;
		ref << endl;
		ref << "    assign a_out = a_in;" << endl;
		ref << endl;
		ref << "endmodule" << endl;
		ref << endl;
		ref << endl;
		ref << "module wire_container_module(" << endl;
		ref << "    input signed [32-1:0] a_in," << endl;
		ref << "    output reg signed [32-1:0] a_out," << endl;
		ref << "    input signed [32-1:0] b_in," << endl;
		ref << "    output signed [32-1:0] b_out);" << endl;
		ref << endl;
		ref << "    wire_module (" << endl;
		ref << "        .a_in(a_in)," << endl;
		ref << "        .a_out(a_out)" << endl;
		ref << "    );" << endl;
		ref << endl;
		ref << "    assign b_out = b_in;" << endl;
		ref << endl;
		ref << "endmodule";

		ser.serialize(ctx);
		//cerr << str.str();
		BOOST_CHECK_EQUAL(str.str(), ref.str());
	}
}

//____________________________________________________________________________//

BOOST_AUTO_TEST_SUITE_END()
