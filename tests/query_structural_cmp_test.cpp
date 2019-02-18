#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE netlistDB_QueryStructuralCmp_test

#include <boost/test/unit_test.hpp>
#include <functional>
#include <iostream>
#include <fstream>
#include <stdint.h>

#include "../src/netlist.h"
#include "../src/statement_if.h"
#include "../src/statement_assignment.h"
#include "../src/hw_type/common.h"
#include "../src/query/query_structural_cmp.h"
#include "../src/query/query_structural_compatible.h"

using namespace std;
namespace tt = boost::test_tools;
using namespace netlistDB;
using namespace netlistDB::hw_type;
using namespace netlistDB::query;

BOOST_AUTO_TEST_SUITE (netlistDB_statement_if_testsuite)

BOOST_AUTO_TEST_CASE( assignements ) {
	Netlist ctx("test");
	auto &a = ctx.sig_in("a", hw_int32);
	auto &b = ctx.sig_out("b", hw_int32);
	auto &c = ctx.sig_in("c", hw_int32);
	auto &assig1 = b(c);
	auto &assig2 = b(c);
	auto &assig3 = b(a);
	auto &assig4 = b[c](a);
	auto &assig5 = b[c](a);

	auto &ifs = If(a)( { &assig1, });

	bool is_same = QueryStructuralCmp::is_same(assig1, assig1);
	BOOST_CHECK_EQUAL(is_same, true);

	is_same = QueryStructuralCmp::is_same(assig1, assig2);
	BOOST_CHECK_EQUAL(is_same, true);

	is_same = QueryStructuralCmp::is_same(assig1, assig3);
	BOOST_CHECK_EQUAL(is_same, false);

	is_same = QueryStructuralCmp::is_same(assig1, assig3);
	BOOST_CHECK_EQUAL(is_same, false);

	is_same = QueryStructuralCmp::is_same(assig1, assig4);
	BOOST_CHECK_EQUAL(is_same, false);

	is_same = QueryStructuralCmp::is_same(assig4, assig5);
	BOOST_CHECK_EQUAL(is_same, true);

	is_same = QueryStructuralCmp::is_same(assig1, ifs);
	BOOST_CHECK_EQUAL(is_same, false);

}

BOOST_AUTO_TEST_CASE( ifs ) {
	Netlist ctx("test");
	auto &a = ctx.sig_in("a", hw_int32);
	auto &b = ctx.sig_out("b", hw_int32);
	auto &c = ctx.sig_in("c", hw_int32);

	auto &ifs1 = If(a)( { &b(c), });
	auto &ifs2 = If(a)( { &b(c), });
	auto &ifs2_b = If(a)( { &c(b), });

	auto &ifs3 = If(a)( { &b(c), &a(c) });
	auto &ifs4 = If(a)( { &b(c), &a(c) });

	auto &ifs5 = If(a)( { &b(c), &a(c) }).Else( { &b(c), &a(c) });
	auto &ifs6 = If(a)( { &b(c), &a(c) }).Else( { &b(c), &a(c) });

	auto &ifs7 = If(a)( { &b(c), &a(c) }).Elif(c, {}).Else( { &b(c), &a(c) });
	auto &ifs8 = If(a)( { &b(c), &a(c) }).Elif(c, {}).Else( { &b(c), &a(c) });

	auto &ifs9 = If(a)( { &b(c), &a(c) }).Elif(c, {&a(c)}).Else( { &b(c), &a(c) });
	auto &ifs10 = If(a)( { &b(c), &a(c) }).Elif(c, {&a(c)}).Else( { &b(c), &a(c) });

	auto &ifs11 = If(a)( { &b(c), &a(c) }).Elif(c, {&a(c)}).Elif(b, {&a(c)}).Else( { &b(c), &a(c) });
	auto &ifs12 = If(a)( { &b(c), &a(c) }).Elif(c, {&a(c)}).Elif(b, {&a(c)}).Else( { &b(c), &a(c) });

	auto &ifs13 = If(a)( { &b(c), &a(c) }).Elif(c, {&a(c)}).Elif(a, {&a(c)}).Else( { &b(c), &a(c) });
	auto &ifs14 = If(a)( { &b(c), &a(c) }).Elif(c, {&a(c)}).Elif(b, {&a(b)}).Else( { &b(c), &a(c) });


	bool is_same = QueryStructuralCmp::is_same(ifs1, ifs2);
	BOOST_CHECK_EQUAL(is_same, true);

	is_same = QueryStructuralCmp::is_same(ifs1, ifs2_b);
	BOOST_CHECK_EQUAL(is_same, false);

	is_same = QueryStructuralCmp::is_same(ifs1, ifs3);
	BOOST_CHECK_EQUAL(is_same, false);

	is_same = QueryStructuralCmp::is_same(ifs3, ifs4);
	BOOST_CHECK_EQUAL(is_same, true);

	is_same = QueryStructuralCmp::is_same(ifs3, ifs5);
	BOOST_CHECK_EQUAL(is_same, false);

	is_same = QueryStructuralCmp::is_same(ifs5, ifs6);
	BOOST_CHECK_EQUAL(is_same, true);

	is_same = QueryStructuralCmp::is_same(ifs6, ifs7);
	BOOST_CHECK_EQUAL(is_same, false);

	is_same = QueryStructuralCmp::is_same(ifs7, ifs8);
	BOOST_CHECK_EQUAL(is_same, true);

	is_same = QueryStructuralCmp::is_same(ifs8, ifs9);
	BOOST_CHECK_EQUAL(is_same, false);

	is_same = QueryStructuralCmp::is_same(ifs9, ifs10);
	BOOST_CHECK_EQUAL(is_same, true);

	is_same = QueryStructuralCmp::is_same(ifs10, ifs11);
	BOOST_CHECK_EQUAL(is_same, false);

	is_same = QueryStructuralCmp::is_same(ifs11, ifs12);
	BOOST_CHECK_EQUAL(is_same, true);

	is_same = QueryStructuralCmp::is_same(ifs12, ifs13);
	BOOST_CHECK_EQUAL(is_same, false);

	is_same = QueryStructuralCmp::is_same(ifs12, ifs14);
	BOOST_CHECK_EQUAL(is_same, false);

}

//____________________________________________________________________________//

BOOST_AUTO_TEST_SUITE_END()
