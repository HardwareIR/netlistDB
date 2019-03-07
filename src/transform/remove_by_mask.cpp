#include <netlistDB/transform/remove_by_mask.h>

#include <netlistDB/utils/erase_if.h>

using namespace netlistDB::parallel_utils;
using namespace std;

namespace netlistDB {
namespace transform {

/* to_update_ep set index helper
 **/
constexpr size_t to_update_ep_index(size_t thread_i, size_t thread_cnt,
		size_t node_i) {
	return thread_i * thread_cnt + (node_i % thread_cnt);
}

bool check_if_any_removed(set<iNode*> * to_update_ep,
		vector<iNode*> * to_delete, size_t thread_cnt) {
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

/*
 * Discover the the object which should be keept and are connected
 * to something which will be removed
 *
 * Apply on items with index % thread_cnt == thread_i
 * (because the removed items tends to be in block
 *  ant the work has to be distributed equally)
 * Put results to separate set so we can avoid synchronization later.
 *
 *  @param to_update_ep (size thread_cnt^2)
 **/
void collect_boundaries_between_deleted_and_keept(
		typename query::QueryTraverse::flag_t * node_to_keep_mask,
		vector<iNode*> nodes, size_t thread_i, size_t thread_cnt,
		size_t size, set<iNode*> * to_update_ep,
		vector<iNode*> & to_delete) {
	for (size_t i = thread_i; i < size; i += thread_cnt) {
		if (not node_to_keep_mask[i]) {
			auto n = nodes[i];
			to_delete.push_back(n);
			n->backward([&](iNode & d) {
				if (node_to_keep_mask[d.index]) {
					// this driver will remain and the deleted segment has to be disconnected
					auto _i = to_update_ep_index(thread_i, thread_cnt,
							d.index);
					to_update_ep[_i].insert(&d);
				}
				return false;
			});
		}
	}
}

/*
 * Disconnect the part of the graph which is being removed
 * from the part which will remain
 *
 * @param _to_update_ep [thread_cnt^2] size array with matrix of sets
 * 		to separate updated objects
 **/
void disconnect_to_remove_connections(
		typename query::QueryTraverse::flag_t * node_to_keep_mask,
		set<iNode*> * _to_update_ep, size_t thread_i, size_t thread_cnt) {
	// collect to_update_ep sets to single vector
	vector<set<iNode*> *> to_update_ep(thread_cnt);
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
					[node_to_keep_mask](iNode* n) {
						return bool(not node_to_keep_mask[n->index]);
					});
		}
	}
}

struct net_index_selector {
	constexpr size_t & operator()(Net * net) {
		return net->net_index;
	}
};

struct node_index_selector {
	constexpr size_t & operator()(iNode * net) {
		return net->index;
	}
};

TransformRemoveByMask::TransformRemoveByMask(
		typename query::QueryTraverse::flag_t node_to_keep_mask[]) :
		node_to_keep_mask(node_to_keep_mask) {
}

void delete_node(iNode * n, Netlist & ctx) {
	auto net = dynamic_cast<Net*>(n);
	if (net) {
		ctx.unregister_node(*net);
	} else {
		ctx.unregister_node(*n);
	}
	delete n;
}

bool TransformRemoveByMask::apply(Netlist & ctx) {
	auto & nodes = ctx.nodes;
	auto cnt = nodes.size();

	size_t thread_cnt = thread_pool.size();
	bool any_removed = false;
	// parallel version
	auto to_update_ep = make_unique<set<iNode*>[]>(
			thread_cnt * thread_cnt);
	auto to_delete = make_unique<vector<iNode*>[]>(thread_cnt);

	// resolve the boundaries between removed and kept part of the graph
	//auto t = new Timer("collect_boundaries_between_deleted_and_keept");
	utils::TaskGroup g(thread_pool);
	for (size_t thr_i = 0; thr_i < thread_cnt; thr_i++) {
		g.run(
				[node_to_keep_mask=node_to_keep_mask, &nodes, thr_i, thread_cnt, cnt,
				_to_update_ep= to_update_ep.get(),
				&_to_delete=to_delete.get()[thr_i]]() {
					collect_boundaries_between_deleted_and_keept(
							node_to_keep_mask, nodes, thr_i, thread_cnt, cnt,
							_to_update_ep, _to_delete);
				});
	}
	g.wait();
	//delete t;

	any_removed = check_if_any_removed(to_update_ep.get(), to_delete.get(),
			thread_cnt);
	if (any_removed) {

		// disconnect the part which is being removed
		// to_update_ep sets are separated by % op and each thread processing disjunct set of nets
		utils::TaskGroup g1(thread_pool);
		//t = new Timer("disconnect_to_remove_connections");
		for (size_t i = 0; i < thread_cnt; i++) {
			g1.run(
					[node_to_keep_mask=node_to_keep_mask,
					 _to_update_ep=to_update_ep.get(), i, thread_cnt, cnt]() {
						disconnect_to_remove_connections(
								node_to_keep_mask, _to_update_ep, i, thread_cnt);
					});
		}
		g1.wait();
		//delete t;

		//t = new Timer("packing");
		// delete the part which is useless
		erase_if<iNode*, node_index_selector>(nodes,
				[node_to_keep_mask=node_to_keep_mask, &ctx](iNode*n) {
					if (bool(not node_to_keep_mask[n->index])) {
						delete_node(n, ctx);
						return true;
					}
					return false;
				});

		erase_if<Net*, net_index_selector>(ctx.nets,
				[](Net*n) {return n == nullptr;});
		//delete t;
	}

	// return true if the netlist was modified
	return any_removed;
}
}
}
