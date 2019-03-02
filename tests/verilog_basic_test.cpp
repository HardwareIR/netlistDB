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

BOOST_AUTO_TEST_CASE( simple_bin_ops ) {
	Netlist ctx("adder");
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

	TransformToHdlFriendly t;
	t.apply(ctx);

	Verilog2001 ser;
	vector<pair<Statement*, string>> expected = {
			{&a_p  , "assign c = a + b;"},
			{&a_m  , "assign c = a - b;"},
			{&a_mul, "assign c = a * b;"},
			{&a_div, "assign c = a / b;"},
			{&a_xor, "assign c = a ^ b;"},
			{&a_or , "assign c = a | b;"},
			{&a_and, "assign c = a & b;"},
			{&a_conc, "assign c2 = {a, b};"},
			{&a_index, "assign c3 = a[a1];"},
	};
	for (auto & t: expected) {
		stringstream str;
		ser.serialize(*t.first, str);
		BOOST_CHECK_EQUAL(str.str(), t.second);
	}
	//ser.serialize(ctx, str);

}

//____________________________________________________________________________//

BOOST_AUTO_TEST_SUITE_END()
