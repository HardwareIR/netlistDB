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
	return n.backward.joined(n.forward);
}

void QueryTraverse::clean_visit_flags() {
	tbb::parallel_for(size_t(0), max_items, [&] (size_t index) {
		visited[index] = false;
	});
	visited_clean = true;
}

void QueryTraverse::traverse(std::vector<iNode*> starts, callback_t callback) {
	if (starts.size() == 0)
		return;
	this->callback = callback;
	if (not visited_clean) {
		clean_visit_flags();
	}
	visited_clean = false;

	tbb::parallel_for_each(starts.begin(), starts.end(), [&] (iNode* item) {
		if (not is_visited(*item)) {
			traverse(*item);
		}});
}

void QueryTraverse::traverse(iNode & n) {
	std::vector<iNode*> stack;
	stack.reserve(std::min(size_t(1024), load_balance_limit));
	stack.push_back(&n);
	size_t to_visit = 0;
	tbb::task_group g;
	while (not stack.empty()) {
		auto ch = stack.back();
		stack.pop_back();
		auto next_items = callback(*ch);

		for (auto item : next_items)
			if (not is_visited(*item)) {
				to_visit++;
				if (to_visit < load_balance_limit) {
					stack.push_back(item); // process next node on this thread
				} else {
					g.run([this, item]() {  // process next node in child task
								traverse(*item);
							});
				}
			}
	}
	g.wait();
}

}
}
