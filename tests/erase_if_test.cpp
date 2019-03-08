#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE netlistDB_erase_if_test

#include <boost/test/unit_test.hpp>
#include <functional>
#include <iostream>
#include <fstream>
#include <random>

#include <netlistDB/utils/erase_if.h>
#include <netlistDB/utils/timer.h>

using namespace netlistDB;
using namespace netlistDB::utils;


BOOST_AUTO_TEST_SUITE( netlistDB_erase_if_testsuite )

struct indexed_int {
	int val;
	int index;
	indexed_int() :
			val(0), index(0) {
	}
	indexed_int(int val, int index) :
			val(val), index(index) {
	}
};

struct indexed_int_index_selector {
	constexpr int & operator()(indexed_int & v) {
		return v.index;
	}

};

BOOST_AUTO_TEST_CASE( simple ) {
	size_t N = 1000;
	std::uniform_int_distribution<> rand_bool(0, 1);
	for (size_t thread_cnt = 1; thread_cnt <= 2; thread_cnt++) {
		// [fixme] thread_cnt not not set properly
		std::mt19937 rand(0);
		std::vector<indexed_int> a(N), a_expected;
		a_expected.reserve(N);
		for (size_t i = 0; i < N; i++) {
			if (rand_bool(rand)) {
				a[i] = indexed_int(i, i);
				a_expected.push_back(indexed_int(i, a_expected.size()));
			} else {
				a[i] = indexed_int(-1, i);
			}
		}

		auto t = new Timer(
				std::string("thread_cnt: ") + std::to_string(thread_cnt));
		erase_if<indexed_int, indexed_int_index_selector>(a,
				[](indexed_int i) {return i.val < 0;});
		delete t;

		BOOST_CHECK_EQUAL(a.size(), a_expected.size());
		for (size_t i = 0; i < a.size(); i++) {
			BOOST_CHECK_EQUAL(a[i].val, a_expected[i].val);
			BOOST_CHECK_EQUAL(a[i].index, a_expected[i].index);
		}
	}

}

//____________________________________________________________________________//

BOOST_AUTO_TEST_SUITE_END()
