#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE netlistDB_verilog_simple_modules_test

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

BOOST_AUTO_TEST_SUITE( netlistDB_verilog_simple_modules_testsuite )

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

BOOST_AUTO_TEST_CASE( simple_ff_module ) {
	Netlist ctx("ff_module");

	auto &clk = ctx.sig_in("clk", hw_bit);
	auto &a_in = ctx.sig_in("a_in", hw_int32);
	auto &a_out = ctx.sig_out("a_out", hw_int32);
	If(clk.rising()) (
		&a_out(a_in)
	);

	TransformToHdlFriendly t;
	t.apply(ctx);

	Verilog2001 ser;
	{
		stringstream str;
		stringstream ref;
		ref << "module ff_module(" << endl;
	    ref << "    input a_in,"<< endl;
	    ref << "    output reg a_out,"<< endl;
	    ref << "    input clk);"<< endl;
        ref << endl;
	    ref << "    always @(posedge clk) begin: diver_of_a_out"<< endl;
	    ref << "        a_out <= a_in;"<< endl;
	    ref << "    end"<< endl;
		ref << endl;
		ref << "endmodule";

		ser.serialize(ctx, str);
		//cerr << str.str() << endl;
		BOOST_CHECK_EQUAL(str.str(), ref.str());
	}
}


BOOST_AUTO_TEST_CASE( mux_module ) {
	Netlist ctx("mux_module");

	auto &sel = ctx.sig_in("sel", hw_int8);
	auto &a_in0 = ctx.sig_in("a_in0", hw_int32);
	auto &a_in1 = ctx.sig_in("a_in1", hw_int32);
	auto &a_in2 = ctx.sig_in("a_in2", hw_int32);
	auto &a_in3 = ctx.sig_in("a_in3", hw_int32);
	auto &a_out = ctx.sig_out("a_out", hw_int32);

	If(sel == 0) (&a_out(a_in0))
	.Elif(sel == 1, &a_out(a_in1))
	.Elif(sel == 2, &a_out(a_in2))
	.Elif(sel == 3, &a_out(a_in3));

	TransformToHdlFriendly t;
	t.apply(ctx);

	Verilog2001 ser;
	{
		stringstream str;
		stringstream ref;
		ref << "module mux_module(" << endl;
		ref << "    input a_in0," << endl;
		ref << "    input a_in1," << endl;
		ref << "    input a_in2," << endl;
		ref << "    input a_in3," << endl;
		ref << "    output reg a_out," << endl;
		ref << "    input sel);" << endl;
        ref << endl;
		ref << "    always @(sel or a_in0 or a_in1 or a_in2 or a_in3) begin: diver_of_a_out" << endl;
		ref << "        if (sel == 8'h00)" << endl;
		ref << "            a_out = a_in0;" << endl;
		ref << "        else if (sel == 8'h01)" << endl;
		ref << "            a_out = a_in1;" << endl;
		ref << "        else if (sel == 8'h02)" << endl;
		ref << "            a_out = a_in2;" << endl;
		ref << "        else if (sel == 8'h03)" << endl;
		ref << "            a_out = a_in3;" << endl;
		ref << endl;
		ref << "    end" << endl;
        ref << endl;
		ref << "endmodule";

		ser.serialize(ctx, str);
		//cerr << str.str() << endl;
		BOOST_CHECK_EQUAL(str.str(), ref.str());
	}
}


//____________________________________________________________________________//

BOOST_AUTO_TEST_SUITE_END()
