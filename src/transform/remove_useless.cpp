#include "remove_useless.h"
#include "../parallel_utils/errase_if.h"
#include "../query/query_traverse.h"
#include "../utils/chained_iterator.h"
#include "remove_by_mask.h"

#include <iostream>

using namespace netlistDB::query;
using namespace netlistDB::parallel_utils;

namespace netlistDB {
namespace transform {

bool TransformRemoveUseless::apply(Netlist & ctx, size_t thread_cnt) {
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
		return n.backward;
	};
	q.traverse(outputs, walk_all_drivers, thread_cnt);

	// the items which was not visited have no effect on output;
	// it the result signal is deleted the driver is deleted as well
	// from this reason we are searching only for signals

	return TransformRemoveByMask(reinterpret_cast<bool*>(q.visited)).apply(ctx, thread_cnt);
}

}
}
