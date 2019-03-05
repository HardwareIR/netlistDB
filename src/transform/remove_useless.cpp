
#include <netlistDB/transform/remove_useless.h>
#include <netlistDB/utils/erase_if.h>
#include <netlistDB/transform/remove_by_mask.h>
#include <netlistDB/query/query_traverse.h>
#include <netlistDB/utils/chained_iterator.h>

//#include "../debug_utils/timer.h"

#include <iostream>

using namespace netlistDB::query;
using namespace netlistDB::parallel_utils;

namespace netlistDB {
namespace transform {

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

	//auto t = new Timer("traversing");
	// discover what was used
	QueryTraverse q(ctx.nodes.size());
	auto walk_all_drivers = [](iNode &n) {
		return n.backward;
	};
	q.traverse(outputs, walk_all_drivers);
	//delete t;

	// the items which was not visited have no effect on output;
	// it the result signal is deleted the driver is deleted as well
	// from this reason we are searching only for signals

	auto visited = reinterpret_cast<typename QueryTraverse::flag_t*>(q.visited);
	return TransformRemoveByMask(visited).apply(ctx);
}

}
}
