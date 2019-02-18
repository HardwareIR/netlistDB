#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE netlistDB_query_enclosure_test

#include <boost/test/unit_test.hpp>
#include <functional>
#include <iostream>
#include <fstream>
#include <stdint.h>

#include <netlistDB/netlist.h>
#include <netlistDB/statement_if.h>
#include <netlistDB/hw_type/common.h>
#include <netlistDB/query/query_sensitivity.h>

using namespace std;
using namespace netlistDB;
using namespace netlistDB::hw_type;
using namespace netlistDB::query;
namespace tt = boost::test_tools;

BOOST_AUTO_TEST_SUITE( netlistDB_query_sensitivity_testsuite )

BOOST_AUTO_TEST_CASE( if_then_else ) {
	Netlist ctx("test");
	auto &a = ctx.sig_in("a", hw_int32);
	auto &b = ctx.sig_in("b", hw_int32);
	auto &c = ctx.sig_out("c", hw_int32);
	vector<iNode*> a_vec( { &a });
	vector<iNode*> ac_vec( { &a, &c });
	{
		auto & i = If(a)( { });
		set<iNode*> seen;
		QuerySensitivity::apply(i, seen);
		BOOST_TEST(i.sens.sensitivity == a_vec, tt::per_element());
		BOOST_CHECK_EQUAL(i.sens.is_completly_event_dependent, false);
		BOOST_CHECK_EQUAL(i.sens.now_is_event_dependent, false);
	}
	{
		set<iNode*> seen;
		auto & i2 = If(a)(&b(c));
		QuerySensitivity::apply(i2, seen);
		BOOST_TEST(i2.sens.sensitivity == ac_vec, tt::per_element());
		BOOST_CHECK_EQUAL(i2.sens.is_completly_event_dependent, false);
		BOOST_CHECK_EQUAL(i2.sens.now_is_event_dependent, false);
	}
	{
		set<iNode*> seen;
		auto & i3 = If(a)(&b(c)).Else(&b(c + a));

		QuerySensitivity::apply(i3, seen);
		BOOST_TEST(i3.sens.sensitivity == ac_vec, tt::per_element());
		BOOST_CHECK_EQUAL(i3.sens.is_completly_event_dependent, false);
		BOOST_CHECK_EQUAL(i3.sens.now_is_event_dependent, false);
	}
	{
		set<iNode*> seen;
		auto & i4 = If(a)(&b(c)).Elif(c, { }).Else(&b(c + a));

		QuerySensitivity::apply(i4, seen);

		BOOST_TEST(i4.sens.sensitivity == ac_vec, tt::per_element());
		BOOST_CHECK_EQUAL(i4.sens.is_completly_event_dependent, false);
		BOOST_CHECK_EQUAL(i4.sens.now_is_event_dependent, false);
	}
	{
		set<iNode*> seen;
		auto & i5 = If(a)(&b(c)).Elif(c, &b(c - a)).Else(&b(c + a));
		QuerySensitivity::apply(i5, seen);

		BOOST_TEST(i5.sens.sensitivity == ac_vec, tt::per_element());
		BOOST_CHECK_EQUAL(i5.sens.is_completly_event_dependent, false);
		BOOST_CHECK_EQUAL(i5.sens.now_is_event_dependent, false);
	}
}

BOOST_AUTO_TEST_CASE( if_then_else_event ) {
	Netlist ctx("test");
	auto &a = ctx.sig_in("a", hw_int32);
	auto &b = ctx.sig_in("b", hw_int32);
	auto &c = ctx.sig_out("c", hw_int32);
	auto &d = ctx.sig_out("d", hw_int32);
	auto &ar = a.rising();
	OperationNode * rising_op = ar.drivers[0];
	vector<iNode*> a_vec( { &a });
	vector<iNode*> ar_vec( { rising_op });
	vector<iNode*> ac_vec( { &a, &c });
	{
		auto & i = If(ar)( { });
		set<iNode*> seen;
		QuerySensitivity::apply(i, seen);
		BOOST_TEST(i.sens.sensitivity == ar_vec, tt::per_element());
		BOOST_CHECK_EQUAL(i.sens.is_completly_event_dependent, true);
		BOOST_CHECK_EQUAL(i.sens.now_is_event_dependent, true);
	}
	{
		set<iNode*> seen;
		auto & i = If(ar)(&b(c));
		QuerySensitivity::apply(i, seen);
		BOOST_TEST(i.sens.sensitivity == ar_vec, tt::per_element());
		BOOST_CHECK_EQUAL(i.sens.is_completly_event_dependent, true);
		BOOST_CHECK_EQUAL(i.sens.now_is_event_dependent, true);
	}
	{
		set<iNode*> seen;
		auto & i = If(a)(&b(c)).Elif(ar, &b(d));
		QuerySensitivity::apply(i, seen);
		BOOST_TEST(i.sens.sensitivity == vector<iNode*>({&a, &c, rising_op}),
				tt::per_element());
		BOOST_CHECK_EQUAL(i.sens.is_completly_event_dependent, false);
		BOOST_CHECK_EQUAL(i.sens.now_is_event_dependent, true);
	}

}

//____________________________________________________________________________//

BOOST_AUTO_TEST_SUITE_END()
