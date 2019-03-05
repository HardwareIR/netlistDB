#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE netlistDB_thread_pool_test

#include <boost/test/unit_test.hpp>
#include <netlistDB/utils/thread_pool.h>

using namespace netlistDB;
using namespace netlistDB::utils;

BOOST_AUTO_TEST_SUITE( netlistDB_thread_pool_testsuite )

//BOOST_AUTO_TEST_CASE( thread_pool_tasks_1 ) {
//	ThreadPool tp(2);
//	std::atomic<size_t> cntr = 0;
//	{
//		auto g = tp.task_group();
//		g.run([&]() {
//			cntr++;
//		});
//		g.wait();
//	}
//	BOOST_CHECK_EQUAL(cntr, 1);
//}

//BOOST_AUTO_TEST_CASE( thread_pool_tasks_2 ) {
//	ThreadPool tp(2);
//	std::atomic<size_t> cntr = 0;
//	{
//		auto g = tp.task_group();
//		for (int i = 0; i < 2; i++) {
//			g.run([&]() {
//				cntr++;
//			});
//		}
//		g.wait();
//	}
//	BOOST_CHECK_EQUAL(cntr, 2);
//}
//
BOOST_AUTO_TEST_CASE( thread_pool_tasks_1024 ) {
	ThreadPool tp(4);
	std::atomic<size_t> cntr = 0;
	{
		auto g = tp.task_group();
		for (int i = 0; i < 1024; i++) {
			g.run([&]() {
				cntr++;
			});
		}
		g.wait();
	}
	std::cout << "first half" << std::endl;
	BOOST_CHECK_EQUAL(cntr, 1024);
	{
		auto g = tp.task_group();
		for (int i = 0; i < 1024; i++) {
			g.run([&]() {
				cntr++;
			});
		}
		g.wait();
	}
	BOOST_CHECK_EQUAL(cntr, 2048);
}

//____________________________________________________________________________//

BOOST_AUTO_TEST_SUITE_END()
