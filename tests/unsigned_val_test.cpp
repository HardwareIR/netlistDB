#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE netlistDB_unsigned_val_test

#include <boost/test/unit_test.hpp>
#include <functional>
#include <iostream>
#include <fstream>
#include <stdint.h>

#include <netlistDB/netlist.h>
#include <netlistDB/statement_if.h>
#include <netlistDB/hw_type/common.h>

using namespace netlistDB;
using namespace netlistDB::hw_type;

BOOST_AUTO_TEST_SUITE( netlistDB_unsigned_val_testsuite )

BOOST_AUTO_TEST_CASE( simple_unsigned_val_test ) {
	Netlist ctx("test");

	Net & c = hw_int64(ctx, aint_t(1));
	BOOST_CHECK_NE(c.val, nullptr);
	auto v = dynamic_cast<HwIntValue*>(c.val);

	BOOST_CHECK_EQUAL(v->value, aint_t(1));
	BOOST_CHECK_EQUAL(v->mask, aint_t(size_t(-1)));
	BOOST_CHECK_EQUAL(v->mask, hw_int64.all_mask);
}

//____________________________________________________________________________//

BOOST_AUTO_TEST_SUITE_END()
