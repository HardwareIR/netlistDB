#include <netlistDB/query/query_traverse.h>

namespace netlistDB {
namespace query {

QueryTraverse::QueryTraverse(size_t max_items) :
		max_items(max_items) {
	visited = new atomic_flag_t[max_items];
	visited_clean = false;
	load_balance_limit = 128;
	callback = nullptr;
}

void QueryTraverse::dummy_callback(iNode &n,
		const std::function<void(iNode &)> & select) {
	n.backward([&select](iNode& n) {
		select(n);
		return false;
	});
	n.forward([&select](iNode& n) {
		select(n);
		return false;
	});
}

void QueryTraverse::clean_visit_flags() {
	utils::TaskGroup g(thread_pool);
	g.parallel_for(0, max_items, [&] (int index) {
		visited[index] = false;
	});
	g.wait();
	visited_clean = true;
}

void QueryTraverse::traverse(std::vector<iNode*> starts,
		const callback_t & callback) {
	if (starts.size() == 0)
		return;
	this->callback = &callback;
	if (not visited_clean) {
		clean_visit_flags();
	}
	visited_clean = false;
	utils::TaskGroup g(thread_pool);
	g.parallel_for(0, starts.size(), [&] (int i) {
		iNode * item = starts[i];
		if (not is_visited(*item)) {
			traverse(*item);
		}});
	g.wait();
}

void QueryTraverse::traverse(iNode & n) {
	std::vector<iNode*> stack;
	stack.reserve(std::min(size_t(1024), load_balance_limit));
	stack.push_back(&n);
	size_t to_visit = 0;
	utils::TaskGroup g(thread_pool);
	while (not stack.empty()) {
		auto ch = stack.back();
		stack.pop_back();

		auto process_item = [&](iNode& item) {
			if (not is_visited(item)) {
				to_visit++;
				if (to_visit < load_balance_limit) {
					stack.push_back(&item); // process next node on this thread
			} else {
				g.run([this, &item]() {  // process next node in child task
							traverse(item);
						});
			}
		}
	}	;
		(*callback)(*ch, process_item);
	}
	g.wait();
}

}
}
