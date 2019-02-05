#include "query_traverse.h"

namespace netlistDB {
namespace query {

QueryTraverse::QueryTraverse(size_t max_items) :
		max_items(max_items) {
	visited = new atomic_flag_t[max_items];
	visited_clean = false;
	load_balance_limit = 128;
	callback = nullptr;
}

iNode::iterator QueryTraverse::dummy_callback(iNode &n) {
	iNode::iterator it;
	// [TODO] chain iterrators instead
	for (auto n : n.backward())
		it.push_back(n);
	for (auto n : n.forward())
		it.push_back(n);
	return it;
}

void QueryTraverse::clean_visit_flags(size_t start, size_t stop) {
	for (size_t i = 0; i < max_items; i++)
		visited[i] = false;
}
void QueryTraverse::clean_visit_flags(size_t thread_cnt) {
	if (thread_cnt > 1) {
		tf::Taskflow tf(thread_cnt);
		size_t step = max_items / thread_cnt;
		for (size_t i = 0; i < max_items; i = std::min(i + step, max_items)) {
			tf.silent_emplace([i, step, this](auto& subflow) {
				clean_visit_flags(i, i+step);
			});
		}
		tf.wait_for_all();
	} else {
		clean_visit_flags(0, max_items);
	}
	visited_clean = true;
}

void QueryTraverse::traverse(std::vector<iNode*> starts, callback_t callback,
		size_t thread_cnt) {
	this->callback = callback;
	if (not visited_clean) {
		clean_visit_flags(thread_cnt);
	}
	visited_clean = false;

	if (thread_cnt > 1) {
		tf::Taskflow tf(thread_cnt);
		for (size_t i = 0; i < starts.size(); i++) {
			auto node = starts[i];
			if (is_visited(*node))
				continue;
			tf.silent_emplace([node, this](auto& subflow) {
				traverse(*node, load_balance_limit, subflow);
			});
		}
		tf.wait_for_all();
	} else {
		for (auto item : starts) {
			if (is_visited(*item))
				continue;
			traverse(*item);
		}
	}
}

void QueryTraverse::traverse(iNode & n, int level_cntr,
		tf::SubflowBuilder& subflow) {
	auto next_nodes = callback(n);
	if (level_cntr <= 0) {
		// if limit is exceeded stop the DFS, dispatch the rest of traversal on this node in next thread
		for (auto ch : next_nodes) {
			if (not is_visited(*ch))
				subflow.silent_emplace([ch, this](tf::SubflowBuilder &subflow) {
					traverse(*ch, load_balance_limit, subflow);
				});
		}
	} else {
		level_cntr--;
		// if limit is not exceeded DFS on this thread
		for (auto ch : next_nodes) {
			if (not is_visited(*ch))
				traverse(*ch, level_cntr, subflow);
		}
	}
}

void QueryTraverse::traverse(iNode & n) {
	std::vector<iNode*> stack;
	stack.reserve(1024);
	stack.push_back(&n);

	while (not stack.empty()) {
		auto ch = stack.back();
		stack.pop_back();
		auto next_items = callback(*ch);
		for (auto item : next_items)
			if (not is_visited(*item))
				stack.push_back(item);
	}
}
}
}
