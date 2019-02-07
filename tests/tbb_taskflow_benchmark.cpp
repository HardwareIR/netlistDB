#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE tbb_taskflow_boost_benchmark_test

#include <boost/test/unit_test.hpp>
#include <functional>
#include <iostream>
#include <fstream>
#include <stdint.h>
#include <random>
#include <chrono>
#include <cstring>
#include <atomic>
#include <tbb/task_group.h>
#include <taskflow/taskflow.hpp>

#include "timer.h"

int fib_tbb(int n) {
	if (n < 2) {
		return n;
	} else {
		int x, y;
		tbb::task_group g;
		g.run([&] {x=fib_tbb(n-1);}); // spawn a task
		g.run([&] {y=fib_tbb(n-2);}); // spawn another task
		g.wait();                // wait for both tasks to complete
		return x + y;
	}
}

int fib_tf(int n, tf::SubflowBuilder& subflow) {
	if (n < 2) {
		return n;
	} else {
		int x, y;
		tbb::task_group g;
		g.run([&] {x=fib_tf(n-1, subflow);}); // spawn a task
		g.run([&] {y=fib_tf(n-2, subflow);}); // spawn another task
		g.wait();                // wait for both tasks to complete
		return x + y;
	}
}

BOOST_AUTO_TEST_SUITE( tbb_taskflow_boost_benchmark_testsuite )

BOOST_AUTO_TEST_CASE( simple_fib ) {
	int N = 40;
	auto * t = new Timer("ttb fib");
	fib_tbb(N);
	delete t;

	auto tf = new tf::Taskflow;
	t = new Timer("ttb fib");
	tf->silent_emplace([N](auto& subflow) {
		fib_tf(N, subflow);
	});
	tf->wait_for_all();
	delete t;
	delete tf;

}

BOOST_AUTO_TEST_SUITE_END()
