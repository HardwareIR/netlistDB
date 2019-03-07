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
#include <netlistDB/query/query_enclosure.h>

using namespace std;
using namespace netlistDB;
using namespace netlistDB::hw_type;
using namespace netlistDB::query;
namespace tt = boost::test_tools;

BOOST_AUTO_TEST_SUITE( netlistDB_query_enclosure_testsuite )

BOOST_AUTO_TEST_CASE( if_then_else ) {
	Netlist ctx("test");
	auto &a = ctx.sig_in("a", hw_int32);
	auto &b = ctx.sig_in("b", hw_int32);
	auto &c = ctx.sig_out("c", hw_int32);

	auto & i = If(a)( { });

	QueryEnclosure::apply(i);
	BOOST_CHECK_EQUAL(i.sens.enclosed_for.size(), 0);
	BOOST_CHECK_EQUAL(i.sens.is_completly_event_dependent, false);
	BOOST_CHECK_EQUAL(i.sens.now_is_event_dependent, false);
	BOOST_CHECK_EQUAL(i.sens.sensitivity.size(), 0);
	BOOST_CHECK_EQUAL(i.sens_extra.elseIf_enclosed_for.size(), 0);
	BOOST_CHECK_EQUAL(i.sens_extra.ifFalse_enclosed_for.size(), 0);
	BOOST_CHECK_EQUAL(i.sens_extra.ifTrue_enclosed_for.size(), 0);

	vector<Net*> b_vec( { &b });
	auto & i2 = If(a)(&b(c));
	QueryEnclosure::apply(i2);
	BOOST_CHECK_EQUAL(i2.sens.enclosed_for.size(), 0);
	BOOST_CHECK_EQUAL(i2.sens.is_completly_event_dependent, false);
	BOOST_CHECK_EQUAL(i2.sens.now_is_event_dependent, false);
	BOOST_CHECK_EQUAL(i2.sens.sensitivity.size(), 0);
	BOOST_CHECK_EQUAL(i2.sens_extra.elseIf_enclosed_for.size(), 0);
	BOOST_CHECK_EQUAL(i2.sens_extra.ifFalse_enclosed_for.size(), 0);
	BOOST_TEST(i2.sens_extra.ifTrue_enclosed_for == b_vec, tt::per_element());

	auto & i3 = If(a)(&b(c)).Else(&b(c + a));

	BOOST_CHECK_EQUAL(i2.sens.enclosed_for.size(), 0);
	QueryEnclosure::apply(i3);
	BOOST_TEST(i3.sens.enclosed_for == b_vec, tt::per_element());
	BOOST_CHECK_EQUAL(i3.sens.is_completly_event_dependent, false);
	BOOST_CHECK_EQUAL(i3.sens.now_is_event_dependent, false);
	BOOST_CHECK_EQUAL(i3.sens.sensitivity.size(), 0);
	BOOST_CHECK_EQUAL(i3.sens_extra.elseIf_enclosed_for.size(), 0);
	BOOST_TEST(i3.sens_extra.ifFalse_enclosed_for == b_vec, tt::per_element());
	BOOST_TEST(i3.sens_extra.ifTrue_enclosed_for == b_vec, tt::per_element());

	auto & i4 = If(a)(&b(c)).Elif(c, { }).Else(&b(c + a));

	QueryEnclosure::apply(i4);
	BOOST_CHECK_EQUAL(i4.sens.enclosed_for.size(), 0);
	BOOST_CHECK_EQUAL(i4.sens.is_completly_event_dependent, false);
	BOOST_CHECK_EQUAL(i4.sens.now_is_event_dependent, false);
	BOOST_CHECK_EQUAL(i4.sens.sensitivity.size(), 0);
	BOOST_CHECK_EQUAL(i4.sens_extra.elseIf_enclosed_for.size(), 1);
	BOOST_TEST(i4.sens_extra.ifFalse_enclosed_for == b_vec, tt::per_element());
	BOOST_TEST(i4.sens_extra.ifTrue_enclosed_for == b_vec, tt::per_element());
    //
	//auto & i5 = If(a)(&b(c)).Elif(c, &b(c - a)).Else(&b(c + a));
	//QueryEnclosure::apply(i5);
	//BOOST_TEST(i3.sens.enclosed_for == b_vec, tt::per_element());
	//BOOST_CHECK_EQUAL(i5.sens.is_completly_event_dependent, false);
	//BOOST_CHECK_EQUAL(i5.sens.now_is_event_dependent, false);
	//BOOST_CHECK_EQUAL(i5.sens.sensitivity.size(), 0);
	//BOOST_CHECK_EQUAL(i5.sens_extra.elseIf_enclosed_for.size(), 1);
	//BOOST_TEST(i5.sens_extra.ifFalse_enclosed_for == b_vec, tt::per_element());
	//BOOST_TEST(i5.sens_extra.ifTrue_enclosed_for == b_vec, tt::per_element());

}

//____________________________________________________________________________//

BOOST_AUTO_TEST_SUITE_END()
