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

using namespace std;
using namespace netlistDB;
using namespace netlistDB::hw_type;
using namespace netlistDB::serializer;

BOOST_AUTO_TEST_SUITE( netlistDB_to_hdl_friendly_testsuite )

BOOST_AUTO_TEST_CASE( simple_bin_ops ) {
	Netlist ctx("test");
	auto &a = ctx.sig_in("a", hw_int32);
	auto &b = ctx.sig_in("b", hw_int32);
	auto &c = ctx.sig_out("c", hw_int32);
	auto &c2 = ctx.sig_out("c2", hw_int64);
	HwInt int4_t(4);
	auto &a1 = ctx.sig_in("a1", int4_t);
	auto &c3 = ctx.sig_out("c3", hw_bit);

	auto & a_p = c(a + b);
	auto & a_m = c(a - b);
	auto & a_mul = c(a * b);
	auto & a_div = c(a / b);
	auto & a_xor = c(a ^ b);
	auto & a_or = c(a | b);
	auto & a_and = c(a & b);
	auto & a_conc = c2(a.concat(b));
	auto & a_index = c3(a[a1]);

	Verilog2001 ser;
	vector<pair<Statement*, string>> expected = {
	    {&a_p,   "assign c = a + b;"},
	    {&a_m,   "assign c = a - b;"},
	    {&a_mul, "assign c = a * b;"},
	    {&a_div, "assign c = a / b;"},
	    {&a_xor, "assign c = a ^ b;"},
	    {&a_or,  "assign c = a | b;"},
	    {&a_and, "assign c = a & b;"},
	    {&a_conc, "assign c2 = {a, b};"},
	    {&a_index, "assign c3 = a[a1];"},
	};

	for (auto & t: expected) {
		stringstream str;
		ser.serialize(*t.first, str);
		BOOST_CHECK_EQUAL(str.str(), t.second);
	}
}

BOOST_AUTO_TEST_CASE( simple_un_ops ) {
	Netlist ctx("test");
	auto &a = ctx.sig_in("a", hw_int32);
	auto &c = ctx.sig_out("c", hw_int32);

	auto & a_n = c(~a);
	auto & a_r = c(a.rising());
	auto & a_f = c(a.falling());

	Verilog2001 ser;
	{
		stringstream str;
		ser.serialize(a_n, str);
		BOOST_CHECK_EQUAL(str.str(), "assign c = ~a;");
	}
	{
		stringstream str;
		BOOST_CHECK_THROW(ser.serialize(a_r, str), std::runtime_error);
		BOOST_CHECK_THROW(ser.serialize(a_f, str), std::runtime_error);
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

		ser.serialize(if0, str);
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

		ser.serialize(if1, str);
		BOOST_CHECK_EQUAL(str.str(), ref.str());
	}
}




//____________________________________________________________________________//

BOOST_AUTO_TEST_SUITE_END()
