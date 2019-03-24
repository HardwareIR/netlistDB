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

#include "components/wire.h"
#include "components/ff.h"
#include "components/mux.h"
#include "components/bram.h"


using namespace std;
using namespace netlistDB;
using namespace netlistDB::hw_type;
using namespace netlistDB::serializer;
using namespace netlistDB::transform;

BOOST_AUTO_TEST_SUITE( netlistDB_verilog_simple_modules_testsuite )

BOOST_AUTO_TEST_CASE( simple_wire_module ) {
	Wire ctx(hw_int32);

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
		ref << "endmodule";
		ser.serialize(ctx);
		BOOST_CHECK_EQUAL(str.str(), ref.str());
	}
}

BOOST_AUTO_TEST_CASE( simple_ff_module ) {
	FF ctx(hw_int32);

	TransformToHdlFriendly t;
	t.apply(ctx);

	{
		stringstream str;
		Verilog2001 ser(str);
		stringstream ref;
		ref << "module ff_module(" << endl;
		ref << "    input signed [32-1:0] a_in," << endl;
		ref << "    output reg signed [32-1:0] a_out," << endl;
		ref << "    input clk);" << endl;
		ref << endl;
		ref << "    always @(posedge clk) begin: diver_of_a_out" << endl;
		ref << "        a_out <= a_in;" << endl;
		ref << "    end" << endl;
		ref << endl;
		ref << "endmodule";

		ser.serialize(ctx);
		//cerr << str.str() << endl;
		BOOST_CHECK_EQUAL(str.str(), ref.str());
	}
}

BOOST_AUTO_TEST_CASE( simple_ff_intern_sig_module ) {
	FF_intern_sig ctx(hw_int32);
	TransformToHdlFriendly t;
	t.apply(ctx);
	{

		stringstream str;
		Verilog2001 ser(str);
		stringstream ref;
		ref << "module ff_intern_sig_module(" << endl;
		ref << "    input signed [32-1:0] a_in," << endl;
		ref << "    output signed [32-1:0] a_out," << endl;
		ref << "    input clk);" << endl;
		ref << endl;
		ref << "    reg signed [32-1:0] reg_0;" << endl;
		ref << endl;
		ref << "    assign a_out = reg_0;" << endl;
		ref << "    always @(posedge clk) begin: diver_of_reg" << endl;
		ref << "        reg_0 <= a_in;" << endl;
		ref << "    end" << endl;
		ref << endl;
		ref << "endmodule";

		ser.serialize(ctx);
		//cerr << str.str() << endl;
		BOOST_CHECK_EQUAL(str.str(), ref.str());
	}
}

BOOST_AUTO_TEST_CASE( mux_module ) {
	Mux4x32 ctx;

	TransformToHdlFriendly t;
	t.apply(ctx);

	{
		stringstream str;
		Verilog2001 ser(str);
		stringstream ref;
		ref << "module mux4x32_module(" << endl;
		ref << "    input signed [32-1:0] a_in0," << endl;
		ref << "    input signed [32-1:0] a_in1," << endl;
		ref << "    input signed [32-1:0] a_in2," << endl;
		ref << "    input signed [32-1:0] a_in3," << endl;
		ref << "    output reg signed [32-1:0] a_out," << endl;
		ref << "    input [8-1:0] sel);" << endl;
		ref << endl;
		ref << "    always @(sel or a_in0 or a_in1 or a_in2 or a_in3) begin: diver_of_a_out"
			<< endl;
		ref << "        if (sel == 8'h00)" << endl;
		ref << "            a_out = a_in0;" << endl;
		ref << "        else if (sel == 8'h01)" << endl;
		ref << "            a_out = a_in1;" << endl;
		ref << "        else if (sel == 8'h02)" << endl;
		ref << "            a_out = a_in2;" << endl;
		ref << "        else if (sel == 8'h03)" << endl;
		ref << "            a_out = a_in3;" << endl;
		ref << "        else" << endl;
		ref << "            a_out = 32'h00000000;" << endl;
		ref << endl;
		ref << "    end" << endl;
		ref << endl;
		ref << "endmodule";

		ser.serialize(ctx);
		//cerr << str.str() << endl;
		BOOST_CHECK_EQUAL(str.str(), ref.str());
	}
}


BOOST_AUTO_TEST_CASE( simple_bram_sp_module ) {
	Bram_sp ctx;

	TransformToHdlFriendly t;
	t.apply(ctx);

	{
		stringstream str;
		Verilog2001 ser(str);
		stringstream ref;
		ref << "module bram_sp_module(" << endl;
		ref << "    input [4-1:0] addr," << endl;
		ref << "    input clk," << endl;
		ref << "    input signed [32-1:0] d_in," << endl;
		ref << "    output reg signed [32-1:0] d_out," << endl;
		ref << "    input en);" << endl;
        ref << endl;
		ref << "    reg signed [32-1:0] mem [0:15];"<< endl;
        ref << endl;
		ref << "    always @(posedge clk) begin: diver_of_d_out" << endl;
		ref << "        d_out <= mem[addr];" << endl;
		ref << "        if (en)" << endl;
		ref << "            mem[addr] <= d_in;" << endl;
        ref << endl;
		ref << "    end" << endl;
        ref << endl;
		ref << "endmodule";

		ser.serialize(ctx);
		//cerr << str.str() << endl;
		BOOST_CHECK_EQUAL(str.str(), ref.str());
	}
}

//____________________________________________________________________________//

BOOST_AUTO_TEST_SUITE_END()
