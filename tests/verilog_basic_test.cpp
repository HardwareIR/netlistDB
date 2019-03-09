#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE netlistDB_verilog_basic_test

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
#include <netlistDB/bit_utils.h>

using namespace std;
using namespace netlistDB;
using namespace netlistDB::hw_type;
using namespace netlistDB::serializer;
using namespace netlistDB::bit_utils;

BOOST_AUTO_TEST_SUITE( netlistDB_verilog_basic_testsuite )

BOOST_AUTO_TEST_CASE( hwint_values ) {
	Netlist ctx("test");
	HwInt hw_uint128(128);
	HwInt hw_uint4(4);

	vector<pair<Net*, string>> expected = {
			{&hw_bit(ctx, 1), "1'b1"},
			{&hw_bit(ctx, 0), "1'b0"},
			{&hw_bit(ctx, 0, 0), "1'bX"},
			{&hw_bit(ctx, 0, 1), "1'b0"},
			{&hw_uint4(ctx, 0, 1), "4'bXXX0"},
			{&hw_uint128(ctx, mask(128)),
			 "128'hffffffffffffffffffffffffffffffff"},
			{&hw_uint128(ctx, mask(128), aint_t(0)),
			 "128'hXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"},
			{&hw_uint128(ctx, mask(64)),
			 "128'h0000000000000000ffffffffffffffff"},
			{&hw_uint128(ctx, mask(64), mask(64)),
			 "128'hXXXXXXXXXXXXXXXXffffffffffffffff"},
			{&hw_uint32(ctx, 0x0123DEAD), "32'h0123dead"}
	};

	Verilog2001 ser;
	for (auto & t: expected) {
		stringstream str;
		ser.serialize_net_usage(*t.first, str);
		BOOST_CHECK_EQUAL(str.str(), t.second);
	}

}

BOOST_AUTO_TEST_CASE( simple_bin_ops ) {
	Netlist ctx("test");
	auto &a = ctx.sig_in("a", hw_int32);
	auto &b = ctx.sig_in("b", hw_int32);
	HwInt int4_t(4);
	auto &a1 = ctx.sig_in("a1", int4_t);

	auto & c_p     = ctx.sig_out("c_p", hw_int32);
	auto & c_m     = ctx.sig_out("c_m", hw_int32);
	auto & c_mul   = ctx.sig_out("c_mul", hw_int32);
	auto & c_div   = ctx.sig_out("c_div", hw_int32);
	auto & c_xor   = ctx.sig_out("c_xor", hw_int32);
	auto & c_or    = ctx.sig_out("c_or", hw_int32);
	auto & c_and   = ctx.sig_out("c_and", hw_int32);
	auto & c_conc  = ctx.sig_out("c_conc", hw_int64);
	auto & c_index = ctx.sig_out("c_index", hw_bit);

	auto & a_p =     c_p(a + b);
	auto & a_m =     c_m(a - b);
	auto & a_mul =   c_mul(a * b);
	auto & a_div =   c_div(a / b);
	auto & a_xor =   c_xor(a ^ b);
	auto & a_or =    c_or(a | b);
	auto & a_and =   c_and(a & b);
	auto & a_conc =  c_conc(a.concat(b));
	auto & a_index = c_index(a[a1]);

	Verilog2001 ser;
	vector<pair<Statement*, string>> expected = {
	    {&a_p,   "assign c_p = a + b;"},
	    {&a_m,   "assign c_m = a - b;"},
	    {&a_mul, "assign c_mul = a * b;"},
	    {&a_div, "assign c_div = a / b;"},
	    {&a_xor, "assign c_xor = a ^ b;"},
	    {&a_or,  "assign c_or = a | b;"},
	    {&a_and, "assign c_and = a & b;"},
	    {&a_conc, "assign c_conc = {a, b};"},
	    {&a_index, "assign c_index = a[a1];"},
	};

	for (auto & t: expected) {
		stringstream str;
		ser.serialize_stm(*t.first, str);
		BOOST_CHECK_EQUAL(str.str(), t.second);
	}
}

BOOST_AUTO_TEST_CASE( simple_un_ops ) {
	Netlist ctx("test");
	auto &a = ctx.sig_in("a", hw_int32);
	auto &c = ctx.sig_out("c", hw_int32);
	auto &c0 = ctx.sig_in("c0", hw_bit);
	auto &c1 = ctx.sig_in("c1", hw_bit);

	auto & a_n = c(~a);
	auto & a_r = c0(a.rising());
	auto & a_f = c1(a.falling());

	Verilog2001 ser;
	{
		stringstream str;
		ser.serialize_stm(a_n, str);
		BOOST_CHECK_EQUAL(str.str(), "assign c = ~a;");
	}
	{
		stringstream str;
		BOOST_CHECK_THROW(ser.serialize_stm(a_r, str), std::runtime_error);
		BOOST_CHECK_THROW(ser.serialize_stm(a_f, str), std::runtime_error);
	}
}

BOOST_AUTO_TEST_CASE( simple_if ) {
	Netlist ctx("test");
	auto &a0 = ctx.sig_in("a0", hw_bit);
	auto &a1 = ctx.sig_in("a1", hw_bit);
	auto &b = ctx.sig_in("b", hw_bit);
	auto &c0 = ctx.sig_out("c0", hw_bit);
	auto &c1 = ctx.sig_out("c1", hw_bit);
	auto &d1 = ctx.sig_out("d1", hw_bit);

	auto & if0 = If(a0)(
		&c0(b)
	).Else(
	   &c0(~b)
	);
	auto & if1 = If(a1)({
		&c1(b),
		&d1(b)
	}).Else({
		&c1(~b),
		&d1(~b)
	});

	auto ind = Verilog2001::INDENT;

	Verilog2001 ser;
	{
		stringstream str;
		stringstream ref;
		ref << "if (a0)" << endl;
		ref << ind << "c0 = b;" << std::endl;
		ref << "else" << std::endl;
		ref << ind << "c0 = ~b;" << std::endl;

		ser.serialize_stm(if0, str);
		BOOST_CHECK_EQUAL(str.str(), ref.str());
	}
	{
		stringstream str;
		stringstream ref;
		ref << "if (a1) begin" << endl;
		ref << ind << "c1 = b;" << std::endl;
		ref << ind << "d1 = b;" << std::endl;
		ref << "end else begin" << std::endl;
		ref << ind << "c1 = ~b;" << std::endl;
		ref << ind << "d1 = ~b;" << std::endl;
		ref << "end";

		ser.serialize_stm(if1, str);
		BOOST_CHECK_EQUAL(str.str(), ref.str());
	}
}

BOOST_AUTO_TEST_CASE( simple_precedence_driven_brackets ) {
	Netlist ctx("test");
	auto &a = ctx.sig_in("a", hw_uint8);
	auto &b = ctx.sig_in("b", hw_uint8);
	auto &c = ctx.sig_in("c", hw_uint8);
	auto &d = ctx.sig_in("d", hw_uint8);

	Verilog2001 ser;
	auto test = [&](Net & n, string ref) {
		stringstream str;
		ser.serialize_net_usage(n, str);
		BOOST_CHECK_EQUAL(str.str(), ref);
	};

	test((a + b) + c, "a + b + c");
	test(a + b + c, "a + b + c");
	test(a + (b + c), "a + (b + c)");
	test((a + b) * c, "(a + b) * c");
	test(a + (b * c), "a + b * c");
	test(a + b * c, "a + b * c");
	test(a[b + c], "a[b + c]");
	test(a + b + c + d, "a + b + c + d");
	test((a + b) + c + d, "a + b + c + d");
	test(((a + b) + c) + d, "a + b + c + d");
	test(a + b + (c + d), "(a + b) + (c + d)");
	test(a + (b + (c + d)), "a + (b + (c + d))");
	test((a + b) + (c + d), "(a + b) + (c + d)");
	test((a + b) * (c + d), "(a + b) * (c + d)");
	test((a * b) + (c * d), "a * b + c * d");
}


//____________________________________________________________________________//

BOOST_AUTO_TEST_SUITE_END()
