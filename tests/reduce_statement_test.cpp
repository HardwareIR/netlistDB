#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE netlistDB_TransformReduceStatement_test

#include <boost/test/unit_test.hpp>
#include <functional>
#include <iostream>
#include <fstream>
#include <stdint.h>

#include <netlistDB/netlist.h>
#include <netlistDB/statement_if.h>
#include <netlistDB/statement_assignment.h>
#include <netlistDB/hw_type/common.h>
#include <netlistDB/transform/reduce_statement.h>
#include <netlistDB/query/query_structural_cmp.h>

using namespace std;
namespace tt = boost::test_tools;
using namespace netlistDB;
using namespace netlistDB::hw_type;
using namespace netlistDB::transform;
using namespace netlistDB::query;

BOOST_AUTO_TEST_SUITE (netlistDB_TransformReduceStatement_testsuite)

BOOST_AUTO_TEST_CASE( ifs ) {
	Netlist ctx("test");
	auto &a = ctx.sig_in("a", hw_int32);
	auto &b = ctx.sig_out("b", hw_int32);
	auto &c = ctx.sig_in("c", hw_int32);
	{
		auto &ifs1 = If(a)( { &b(c), });
		auto &ifs2 = If(a)( { &c(b), });
		vector<Statement*> inp = { &ifs1, &ifs2 };
		vector<Statement*> res;
		TransformReduceStatement::apply(inp, res);
		BOOST_CHECK_EQUAL(res.size(), 1);

		auto &ifs_expected = If(a)( { &b(c), &c(b), });
		bool is_same = QueryStructuralCmp::is_same(*res[0], ifs_expected);
		BOOST_CHECK_EQUAL(is_same, true);

	}
	{
		auto &ifs1 = If(a)( { &b(c), });
		auto &ifs2 = If(a)({}).Else( { &c(b), });
		vector<Statement*> inp = { &ifs1, &ifs2 };
		vector<Statement*> res;
		TransformReduceStatement::apply(inp, res);
		BOOST_CHECK_EQUAL(res.size(), 2);

		auto &ifs1_b = If(a)( { &b(c), });
		auto &ifs2_b = If(a)({}).Else( { &c(b), });
		bool is_same = QueryStructuralCmp::is_same(*res[0], ifs1_b);
		BOOST_CHECK_EQUAL(is_same, true);

		is_same = QueryStructuralCmp::is_same(*res[1], ifs2_b);
		BOOST_CHECK_EQUAL(is_same, true);
	}
}

//____________________________________________________________________________//

BOOST_AUTO_TEST_SUITE_END()
