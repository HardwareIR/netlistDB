#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE netlistDB_test

#include <boost/test/unit_test.hpp>
#include <functional>
#include <iostream>
#include <fstream>
#include <stdint.h>
#include <random>
#include <chrono>
#include <cstring>
#include <atomic>
#include <taskflow/taskflow.hpp>
#include <boost/range/join.hpp>

#include "test_graphs.h"

using namespace netlistDB;

BOOST_AUTO_TEST_SUITE( netlistDB_testsuite )

class QueryTraverse {
public:
	using atomic_flag_t = std::atomic<bool>;
	using callback_t = std::function<iNode::iterator(iNode&)>;
	atomic_flag_t * visited;
	bool visited_clean;
	size_t load_balance_limit;
	callback_t callback;

	QueryTraverse(size_t max_items) {
		visited = new atomic_flag_t[max_items] { false, };
		visited_clean = true;
		load_balance_limit = 128;
		callback = nullptr;
	}

	static iNode::iterator dummy_callback(iNode &n) {
		return boost::join(n.backward(), n.forward());
	}

	void traverse(std::vector<iNode*> starts, callback_t callback,
			size_t thread_cnt = 1) {
		this->callback = callback;
		if (thread_cnt > 1) {
			tf::Taskflow tf(thread_cnt);
			for (size_t i = 0; i < starts.size(); i++) {
				tf.silent_emplace([node=starts[i], this](auto& subflow) {
					traverse(*node, load_balance_limit, subflow);
				});
			}
			tf.wait_for_all();  // block until finished
		} else {
			for (auto item: starts) {
				traverse(*item);
			}
		}
	}

	void traverse(iNode & n, int level_cntr, tf::SubflowBuilder& subflow) {
		if (visited[n.index].exchange(true))
			return;
		if (level_cntr <= 0) {
			// if limit is exceeded stop the DFS, dispatch the rest of traversal on this node in next thread
			for (auto ch : callback(n)) {
				subflow.silent_emplace(
						[ch, this](tf::SubflowBuilder &subflow) {
							traverse(*ch, load_balance_limit, subflow);
						});
			}
		} else {
			level_cntr--;
			// if limit is not exceeded DFS on this thread
			for (auto ch : callback(n)) {
				traverse(*ch, level_cntr, subflow);
			}
		}
	}
	/*
	 * Traverse using the DFS and vector as a stack
	 *
	 * @note performance same as recursive version
	 **/
	void traverse(iNode & n) {
		if (visited[n.index].exchange(true))
			return;

		std::vector<iNode*> stack;
		stack.reserve(1024);
		stack.push_back(&n);

		while (not stack.empty()) {
			auto ch = stack.back();
			stack.pop_back();

			for (auto b : ch->backward())
				if (not visited[b->index].exchange(true))
					stack.push_back(b);

			for (auto f : ch->forward())
				if (not visited[f->index].exchange(true))
					stack.push_back(f);
		}
	}
};

void traversal(std::vector<iNode*>& src, size_t thred_cnt) {
	auto start = std::chrono::system_clock::now();

	auto end = std::chrono::system_clock::now();
	std::cout << "threads " << thred_cnt << " Tf runtime: "
			<< std::chrono::duration_cast<std::chrono::milliseconds>(
					end - start).count() << " ms" << std::endl;
}

void tf_test(std::vector<iNode*> & outputs, QueryTraverse& q, size_t obj_cnt,
		size_t thread_cnt) {
	std::atomic<size_t> visited_cnt = 0;
	q.traverse(outputs, [&visited_cnt](iNode & n) {
		visited_cnt++;
		return QueryTraverse::dummy_callback(n);
	}, thread_cnt);
	BOOST_CHECK_EQUAL(visited_cnt, obj_cnt);

}

Netlist* build_graph(std::vector<iNode*> & outputs, size_t N,
		size_t expected_node_cnt) {
	auto ctx = new Netlist("example_circuit");
	std::mt19937 rand(0);

	auto start = std::chrono::system_clock::now();
	build_random_circuit(*ctx, N, N, N, N, rand);
	BOOST_CHECK_EQUAL(ctx->obj_seq_num, expected_node_cnt);

	auto end = std::chrono::system_clock::now();
	std::cout << "construction time: "
			<< std::chrono::duration_cast<std::chrono::milliseconds>(
					end - start).count() << " ms" << std::endl;

	for (auto & n : ctx->nets) {
		if (n->direction == Direction::DIR_OUT) {
			outputs.push_back(n);
		}
	}
	return ctx;
}

BOOST_AUTO_TEST_CASE( simple_traversal_100 ) {
	size_t N = 100;
	size_t expected_node_cnt = 20090;
	std::vector<iNode*> outputs;
	Netlist * ctx = build_graph(outputs, N, expected_node_cnt);

	for (size_t i = 1; i <= std::thread::hardware_concurrency(); i++) {
		tf_test(*ctx, outputs, i);
	}
}
//
//BOOST_AUTO_TEST_CASE( simple_traversal_1000 ) {
//	size_t N = 1000;
//	size_t expected_node_cnt = 1983146;
//	std::vector<iNode*> outputs;
//	Netlist * ctx = build_graph(outputs, N, expected_node_cnt);
//
//	for (size_t i = 1; i <= std::thread::hardware_concurrency(); i++) {
//		tf_test(*ctx, outputs, i);
//	}
//}

//BOOST_AUTO_TEST_CASE( simple_traversal_2000 ) {
//	size_t N = 2000;
//	size_t expected_node_cnt = 1983146;
//	std::vector<iNode*> outputs;
//	Netlist * ctx = build_graph(outputs, N, expected_node_cnt);
//
//	for (size_t i = 8; i <= std::thread::hardware_concurrency(); i++) {
//		tf_test(*ctx, outputs, i);
//	}
//}
//____________________________________________________________________________//

BOOST_AUTO_TEST_SUITE_END()
