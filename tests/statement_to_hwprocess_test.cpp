
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE netlistDB_statement_to_hwprocess_test

#include <boost/test/unit_test.hpp>
#include <functional>
#include <iostream>
#include <fstream>
#include <stdint.h>

#include <netlistDB/netlist.h>
#include <netlistDB/statement_if.h>
#include <netlistDB/hw_type/common.h>
#include <netlistDB/query/query_structural_cmp.h>
#include <netlistDB/transform/mark_inter_process_nets.h>
#include <netlistDB/transform/statement_to_hwprocess.h>

using namespace std;
using namespace netlistDB;
using namespace netlistDB::hw_type;
using namespace netlistDB::transform;
using namespace netlistDB::query;

BOOST_AUTO_TEST_SUITE( netlistDB_statement_to_hwprocess_testsuite )

BOOST_AUTO_TEST_CASE( simple_mux ) {
	Netlist ctx("mux");
	auto &a = ctx.sig_in("a", hw_bit);
	auto &b = ctx.sig_in("b", hw_bit);
	auto &c = ctx.sig_in("c", hw_bit);
	auto &d = ctx.sig_in("c", hw_bit);


	auto & i = If(d)({
		&a(b),
	}).Else({
		&a(c),
	});

	vector<Statement*> inp = {&i};
	vector<HwProcess*> res;
	TransformMarkInterProcessNets mp;
	mp.apply(ctx);

	TransformStatementToHwProcess::apply(inp, res, true);
	TransformStatementToHwProcess::reduce(res);

	BOOST_CHECK_EQUAL(res.size(), 1);
	BOOST_CHECK_EQUAL(res[0]->statements.size(), 1);
	BOOST_CHECK_EQUAL(res[0]->statements[0], &i);

}

BOOST_AUTO_TEST_CASE( simple_mux_comb_loop ) {
	Netlist ctx("mux");
	auto &a = ctx.sig_in("a", hw_bit);
	auto &b = ctx.sig_in("b", hw_bit);
	auto &c = ctx.sig_in("c", hw_bit);

	auto & i = If(a)({
		&a(b),
	}).Else({
		&a(c),
	});

	vector<Statement*> inp = {&i};
	vector<HwProcess*> res;
	TransformMarkInterProcessNets mp;
	mp.apply(ctx);

	BOOST_CHECK_THROW(
			TransformStatementToHwProcess::apply(inp, res, true),
			TransformStatementToHwProcess::CombinationalLoop);
	TransformStatementToHwProcess::reduce(res);

	BOOST_CHECK_EQUAL(res.size(), 0);
}

BOOST_AUTO_TEST_CASE( simple_mux_merging ) {
	Netlist ctx("mux");
	auto &a = ctx.sig_in("a", hw_bit);
	auto &b = ctx.sig_in("b", hw_bit);
	auto &c = ctx.sig_in("c", hw_bit);
	auto &d = ctx.sig_in("d", hw_bit);
	auto &e = ctx.sig_in("e", hw_bit);
	auto &f = ctx.sig_in("f", hw_bit);

	auto & i0 = If(f)({
		&a(b),
	}).Else({
		&a(c),
	});

	auto & i1 = If(f)({
		&d(b),
	}).Else({
		&d(c),
	});

	auto & i2 = If(f)({
		&e(b),
	}).Else({
		&e(c),
	});


	vector<Statement*> inp = {&i0, &i1, &i2};
	vector<HwProcess*> res;
	TransformMarkInterProcessNets mp;
	mp.apply(ctx);
	for (auto i: inp)
		TransformStatementToHwProcess::apply({i}, res, true);
	BOOST_CHECK_EQUAL(res.size(), 3);
	TransformStatementToHwProcess::reduce(res);


	BOOST_CHECK_EQUAL(res.size(), 1);
	BOOST_CHECK_EQUAL(res[0]->statements.size(), 1);
	auto i = dynamic_cast<IfStatement*>(res[0]->statements[0]);
	BOOST_CHECK_NE(i, nullptr);
	BOOST_CHECK_EQUAL(i, &i0); // but i0 is now something different than it was before
	BOOST_CHECK_EQUAL(i->condition, &f);
	BOOST_CHECK_EQUAL(i->ifTrue.size(), 3);
	BOOST_CHECK_EQUAL(i->ifFalse.size(), 3);

	auto& i_exp = If(f) ({
		&a(b),
		&d(b),
		&e(b),
	}).Else({
		&a(c),
		&d(c),
		&e(c),
	});
	BOOST_CHECK(QueryStructuralCmp::is_same(*i, i_exp));

}


//____________________________________________________________________________//

BOOST_AUTO_TEST_SUITE_END()
