#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE netlistDB_utils_ordered_set_test

#include <boost/test/unit_test.hpp>
#include <functional>
#include <iostream>
#include <fstream>
#include <stdint.h>
#include "../src/utils/ordered_set.h"

using namespace netlistDB::utils;

BOOST_AUTO_TEST_SUITE( netlistDB_utils_ordered_set_testsuite )

BOOST_AUTO_TEST_CASE( simple_insert) {
	OrderedSet<int> s;

	s.push_back(0);
	BOOST_CHECK_EQUAL(s.size(), 1);

	s.push_back(0);
	BOOST_CHECK_EQUAL(s.size(), 1);

	for (size_t i = 0; i < 40; i++) {
		s.push_back(i);
	}
	BOOST_CHECK_EQUAL(s.size(), 40);
	for (size_t i = 0; i < 40; i++) {
		s.push_back(i);
	}
	BOOST_CHECK_EQUAL(s.size(), 40);
}

BOOST_AUTO_TEST_CASE( simple_reinterpret_to_vector) {
	OrderedSet<int> s;

	for (size_t i = 0; i < 40; i++) {
		s.push_back(i);
	}
	auto & v = *reinterpret_cast<std::vector<int>*>(&s);
	BOOST_CHECK_EQUAL(v.size(), 40);
	for (size_t i = 0; i < 40; i++) {
		BOOST_CHECK_EQUAL(v.at(i), i);
	}
}

//____________________________________________________________________________//

BOOST_AUTO_TEST_SUITE_END()
