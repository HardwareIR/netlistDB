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
std::vector<int> a = { 1, 2, 3 };
std::vector<int> b = { 4, 5 };
std::vector<int> c = { 6 };
std::vector<int> d = { 7 };

BOOST_AUTO_TEST_CASE( simple_chained_iteration ) {
	ChainedSequence<int> it( { &a, &b, &c, &d });

	size_t i = 0;
	for (auto & item : it) {
		//std::cout << item << std::endl;
		BOOST_CHECK_EQUAL(item, i + 1);
		i++;
	}
	BOOST_CHECK_EQUAL(i, 7);

	i = 0;
	for (auto & item : it) {
		//std::cout << item << std::endl;
		BOOST_CHECK_EQUAL(item, i + 1);
		i++;
	}
	BOOST_CHECK_EQUAL(i, 7);

}

BOOST_AUTO_TEST_CASE( simple_join ) {
	ChainedSequence<int> it2( { &a, &b });
	ChainedSequence<int> it3( { &c, &d });

	size_t i = 0;
	for (auto & item : it2) {
		BOOST_CHECK_EQUAL(item, i + 1);
		i++;
	}
	BOOST_CHECK_EQUAL(i, 5);

	i = 0;
	for (auto & item : it3) {
		BOOST_CHECK_EQUAL(item, i + 6);
		i++;
	}
	BOOST_CHECK_EQUAL(i, 2);

	i = 0;
	for (auto & item : it2.joined(it3)) {
		BOOST_CHECK_EQUAL(item, i + 1);
		i++;
	}
	BOOST_CHECK_EQUAL(i, 7);
}

BOOST_AUTO_TEST_CASE( simple_single_items ) {
	int a = 1;
	int b = 2;
	int c = 3;
	int d = 4;

	ChainedSequence<int> it2;
	it2.push_back(&a);
	it2.push_back(&b);
	it2.push_back(&c);
	it2.push_back(&d);

	size_t i = 0;
	for (auto & item : it2) {
		BOOST_CHECK_EQUAL(item, i + 1);
		i++;
	}
	BOOST_CHECK_EQUAL(i, 4);
}

BOOST_AUTO_TEST_CASE( simple_empty_vectors) {
	std::vector<int> empty[5];
	ChainedSequence<int> it( { &empty[0], &a, &empty[1], &b, &empty[2], &c,
			&empty[3], &d, &empty[4], });

	size_t i = 0;
	for (auto & item : it) {
		//std::cout << item << std::endl;
		BOOST_CHECK_EQUAL(item, i + 1);
		i++;
	}
	BOOST_CHECK_EQUAL(i, 7);
}

BOOST_AUTO_TEST_CASE( simple_single_items_and_vec ) {
	int _a = 1;
	int _b = 2;
	int _c = 3;
	int _d = 4;
	int _e = 5;

	ChainedSequence<int> it;
	it.push_back(&_a);
	it.push_back(&_b);
	it.push_back(&_c);
	it.push_back(&_d);
	it.push_back(&_e);
	it.push_back(&c);
	it.push_back(&d);

	size_t i = 0;
	for (auto & item : it) {
		//std::cout << item << std::endl;
		BOOST_CHECK_EQUAL(item, i + 1);
		i++;
	}
	BOOST_CHECK_EQUAL(i, 7);
}

//____________________________________________________________________________//

BOOST_AUTO_TEST_SUITE_END()
