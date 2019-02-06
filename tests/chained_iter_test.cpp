#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE netlistDB_test

#include <boost/test/unit_test.hpp>
#include <functional>
#include <iostream>
#include <fstream>
#include <stdint.h>
#include "../src/utils/chained_iterator.h"

using namespace netlistDB::utils;

BOOST_AUTO_TEST_SUITE( netlistDB_testsuite )

BOOST_AUTO_TEST_CASE( simple_chained_iteration ) {

	std::vector<int> a = { 1, 2, 3 };
	std::vector<int> b = { 4, 5 };
	std::vector<int> c = { 6 };
	std::vector<int> d = { 7 };

	ChaindedIter<int> it( { &a, &b, &c, &d }, 0, 7);

	size_t i = 0;
	for (auto & item : it) {
		//std::cout << item << std::endl;
		BOOST_CHECK_EQUAL(item, i + 1);
		i++;
	}
	BOOST_CHECK_EQUAL(i, 7);


	ChaindedIter<int> it2( { &a, &b, &c, &d }, 1, 6);

	i = 1;
	for (auto item : it2) {
		//std::cout << item << std::endl;
		BOOST_CHECK_EQUAL(item, i + 1);
		i++;
	}
	BOOST_CHECK_EQUAL(i, 6);

	ChaindedIter<int> it3( { &a, &b, &c, &d }, 6, 1);
	i = 6;
	for (auto item : it3) {
		//std::cout << item << std::endl;
		BOOST_CHECK_EQUAL(item, i + 1);
		i++;
	}
	BOOST_CHECK_EQUAL(i, 7);
}

//____________________________________________________________________________//

BOOST_AUTO_TEST_SUITE_END()
