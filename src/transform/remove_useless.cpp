#include <parallel_utils/inclusive_scan.h>
#include "remove_useless.h"
#include "../query/query_traverse.h"
#include "../parallel_utils/errase.h"

#include <iostream>

using namespace netlistDB::query;
using namespace netlistDB::parallel_utils;

namespace netlistDB {
namespace transform {

constexpr size_t to_update_ep_index(size_t thread_i, size_t thread_cnt,
		size_t node_i) {
	return thread_i * thread_cnt + (node_i % thread_cnt);
}

bool check_if_any_removed(std::set<iNode*> * to_update_ep,
		std::vector<iNode*> * to_delete, size_t thread_cnt) {
	for (size_t i = 0; i < thread_cnt; i++) {
		if (to_delete[i].size()) {
			return true;
		}
		for (size_t i2 = 0; i2 < thread_cnt; i2++) {
			if (to_update_ep[to_update_ep_index(i, thread_cnt, i2)].size()) {
				return true;
			}
		}
	}
	return false;
}

// [TODO] cancel atomic if synchr. is not required
/*
 * Discover the
 *
 * Apply on items with index % thread_cnt == thread_i
 * (because the removed items tends to be in block
 *  ant the work has to be distributed equally)
 * Put results to separate set so we can avoid synchronization later.
 *
 *  @param to_update_ep (size thread_cnt^2)
 **/
void collect_boundaries_between_deleted_and_keept(
		QueryTraverse::atomic_flag_t * visited, std::vector<iNode*> nodes,
		size_t thread_i, size_t thread_cnt, size_t size,
		std::set<iNode*> * to_update_ep, std::vector<iNode*> & to_delete) {
	for (size_t i = thread_i; i < size; i += thread_cnt) {
		if (not visited[i]) {
			auto n = nodes[i];
			to_delete.push_back(n);
			for (auto d : n->backward()) {
				if (visited[d->index]) {
					// this driver will remain and the deleted segment has to be disconnected
					auto _i = to_update_ep_index(thread_i, thread_cnt,
							d->index);
					to_update_ep[_i].insert(d);
				}
			}
		}
	}
}

/*
 * Disconnect the part of the graph which is being removed
 * from the part which will remain
 *
 **/
void disconnect_to_remove_connections(QueryTraverse::atomic_flag_t * visited,
		std::set<iNode*> * _to_update_ep, size_t thread_i, size_t thread_cnt,
		size_t size) {
	// collect to_update_ep sets to single vector
	std::vector<std::set<iNode*> *> to_update_ep(thread_cnt);
	for (size_t i = 0; i < thread_cnt; i++) {
		to_update_ep[i] = &_to_update_ep[to_update_ep_index(i, thread_cnt,
				thread_i)];
	}

	// disconnect the unique items from all sets
	for (size_t i = 0; i < to_update_ep.size(); i++) {
		for (auto boundary_item : *to_update_ep[i]) {
			for (size_t i2 = i + 1; i2 < to_update_ep.size(); i2++) {
				// remove the item from all other sets so we clean it only once
				to_update_ep[i2]->erase(boundary_item);
			}

			// disconnect the deleted part of the circuit
			dynamic_cast<Net*>(boundary_item)->forward_disconnect(
					[visited](iNode* n) {
						return bool(not visited[n->index]);
					});
		}
	}
}

/* Remove item from vector by
 *
 * @return new size of array
 **/
template<typename mask_index_predicate>
size_t pack(tf::Taskflow & tf, iNode* input, iNode* output, size_t item_cnt,
		QueryTraverse::atomic_flag_t * keep_flags) {
	auto index_in = std::make_unique<int[]>(item_cnt);
	auto index_tmp = std::make_unique<int[]>(item_cnt);
	auto index_out = std::make_unique<int[]>(item_cnt);
	size_t step = item_cnt / tf.num_workers();

	// [TODO] maybe just cast can be sufficient but we
	// probably still need a copy of the original array to not destroy it

	// initialize index
	for (size_t w = 0; w < tf.num_workers(); w++) {
		tf.silent_emplace(
				[w, index=index_in.get(), item_cnt, step, input, keep_flags]() {
					size_t last = std::min((w+1)*step, item_cnt);
					for (size_t i = w*step; i < last; i++) {
						auto _i = mask_index_predicate {}(input[i]);
						index[i] = int(keep_flags[_i]);
					}
				});
	}
	tf.wait_for_all();

	inclusive_scan<int>(index_in.get(), index_out.get(), index_tmp.get(),
			item_cnt, tf);

	size_t new_item_cnt = index_out.get()[item_cnt - 1];
	if (new_item_cnt == 0)
		return 0;

	step = item_cnt / tf.num_workers();
	// put items on their palaces
	for (size_t w = 0; w < tf.num_workers(); w++) {
		tf.silent_emplace(
				[w, index=index_out.get(), new_item_cnt, step, input, keep_flags]() {
					size_t last = std::min((w+1)*step, new_item_cnt);
					for (size_t i = w*step; i < last; i++) {
						auto _i = mask_index_predicate {}(input[i]);
						index[i] = int(keep_flags[_i]);
					}
				});
	}
	tf.wait_for_all();

	return new_item_cnt;
}

//void remove_deleted_nodes_and_nets(tf::Taskflow & tf,
//		std::vector<iNode*> & nodes, std::vector<Net*> & nets) {
//	std::vector<iNode*> nodes_tmp(nodes.size());
//	std::vector<Net*> nets_tmp(nets.size());
//
//	auto nodes_index = std::make_unique<int[]>(nodes.size());
//	auto nets_index = std::make_unique<int[]>(nets.size());
//
//}

struct net_index_selector {
	constexpr size_t & operator()(Net * net) {
		return net->net_index;
	}
};

bool TransformRemoveUseless::apply(Netlist & ctx) {
	if (ctx.nodes.size() == 0)
		return false;

	// collect outputs
	std::vector<iNode*> outputs;
	for (auto & n : ctx.nets) {
		if (n->direction == Direction::DIR_OUT) {
			outputs.push_back(n);
		}
	}

	// discover what was used
	QueryTraverse q(ctx.nodes.size());
	auto walk_all_drivers = [](iNode &n) {
		return n.backward();
	};
	q.traverse(outputs, walk_all_drivers);

	// the items which was not visited have no effect on output;
	// it the result signal is deleted the driver is deleted as well
	// from this reason we are searching only for signals
	auto visited = q.visited;
	auto & nodes = ctx.nodes;
	auto cnt = nodes.size();
	auto thread_cnt = std::thread::hardware_concurrency();
	if (thread_cnt > 1) {
		// parallel version
		tf::Taskflow tf(thread_cnt);
		auto to_update_ep = std::make_unique<std::set<iNode*>[]>(
				thread_cnt * thread_cnt);
		auto to_delete = std::make_unique<std::vector<iNode*>[]>(thread_cnt);

		// resolve the boundaries between removed and kept part of the graph
		for (size_t thr_i = 0; thr_i < thread_cnt; thr_i++) {
			tf.silent_emplace([visited, &nodes, thr_i, thread_cnt, cnt,
			_to_update_ep= to_update_ep.get(),
			&_to_delete=to_delete.get()[thr_i]]() {
				collect_boundaries_between_deleted_and_keept(
						visited, nodes, thr_i, thread_cnt, cnt,
						_to_update_ep, _to_delete);
			});
		}
		tf.wait_for_all();

		// disconnect the part which is being removed
		for (size_t i = 0; i < thread_cnt; i++) {
			tf.silent_emplace(
					[visited, _to_update_ep=to_update_ep.get(), i, thread_cnt, cnt]() {
						disconnect_to_remove_connections(
								visited, _to_update_ep, i, thread_cnt, cnt);
					});
		}
		tf.wait_for_all();

		// return true if the netlist was modified
		bool any_removed = check_if_any_removed(to_update_ep.get(),
				to_delete.get(), thread_cnt);

		if (any_removed) {
			// delete the part which is useless
			errase_if<iNode*>(nodes, [visited, &ctx](iNode*n) {
				if (bool(not visited[n->index])) {
					auto net = dynamic_cast<Net*>(n);
					if (net) {
						ctx.unregister_node(*net);
					} else {
						ctx.unregister_node(*n);
					}
					delete n;
					return true;
				}
				return false;
			});
			size_t i = 0;
			for (auto n : nodes) {
				n->index = i;
				i++;
			}

			compress_vec<Net, net_index_selector>(ctx.nets);

			//ctx.integrty_assert();
		}
		return any_removed;
	} else {
		// sequential version
		std::set<iNode*> to_update_ep;
		std::vector<iNode*> to_delete;

		// resolve the boundaries between removed and kept part of the graph
		collect_boundaries_between_deleted_and_keept(visited, nodes, 0, 1, cnt,
				&to_update_ep, to_delete);

		// disconnect the part which is being removed
		disconnect_to_remove_connections(visited, &to_update_ep, 0, 1, cnt);

		// return true if the netlist was modified
		bool any_removed = to_delete.size() or to_update_ep.size();
		if (any_removed) {
			std::remove_if(nodes.begin(), nodes.end(), [visited](iNode*n) {
				return bool(not visited[n->index]);
			});
			// delete the part which is useless
			for (auto n : to_delete) {
				auto net = dynamic_cast<Net*>(n);
				if (net) {
					ctx.unregister_node(*net);
				} else {
					ctx.unregister_node(*n);
				}
				delete n;
			}
		}

		return any_removed;
	}
}

}
}
