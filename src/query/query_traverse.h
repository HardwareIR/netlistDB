#pragma once
#include "../netlist.h"
#include <taskflow/taskflow.hpp>

namespace netlistDB {
namespace query {

class QueryTraverse {
public:
	using atomic_flag_t = std::atomic<bool>;
	using callback_t = std::function<iNode::iterator(iNode&)>;
	atomic_flag_t * visited;
	bool visited_clean;
	size_t load_balance_limit;
	callback_t callback;
	size_t max_items;

	QueryTraverse(size_t max_items);

	inline bool is_visited(iNode &n) {
		//return visited[n.index].exchange(true);
		return visited[n.index].exchange(true);
	}

	static iNode::iterator dummy_callback(iNode &n);

	void clean_visit_flags(size_t start, size_t stop);
	void clean_visit_flags(size_t thread_cnt);

	/*
	 * Traverse graph from starts and call callback on each node exactly once
	 * @param starts the nodes where the search should start
	 * @param callback the callback function called on each vistited node (exactly once)
	 * @param thread_cnt the number of threads used for processing (1, 0 = processing on this core only)
	 * */
	void traverse(std::vector<iNode*> starts, callback_t callback,
			size_t thread_cnt = 1);

	/*
	 * Traverse using limited DFS, if limit is exceeded the rest children are processed
	 * new task
	 *
	 * @param n node to process
	 * @param level_cntr counter of remaining levels to split the task
	 * @param subflow the subflow for new tasks (thread pool like object)
	 * */
	void traverse(iNode & n, int level_cntr, tf::SubflowBuilder& subflow);
	/*
	 * Traverse using the DFS and vector as a stack
	 *
	 * @note performance same as recursive version
	 **/
	void traverse(iNode & n);
};
}
}
