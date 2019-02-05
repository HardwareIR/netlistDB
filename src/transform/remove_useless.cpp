#include "remove_useless.h"
#include "../query/query_traverse.h"

namespace netlistDB {
namespace transform {

bool TransformRemoveUseless::apply(Netlist & ctx) {
	if (ctx.nets.size() == 0)
		return false;

	std::vector<iNode*> outputs;

	for (auto & n : ctx.nets) {
		if (n->direction == Direction::DIR_OUT) {
			outputs.push_back(n);
		}
	}

	query::QueryTraverse q(ctx.nodes.size());

	auto walk_all_drivers = [](iNode &n) {
		return n.backward();
	};

	q.traverse(outputs, walk_all_drivers);

	// the items which was not visited have no effect on output;
	// it the result signal is deleted the driver is deleted as well
	// from this reason we are searching only for signals
	auto visited = q.visited;
	std::vector<Net*> deleted;
	for (auto net : ctx.nets) {
		if (not visited[net->index]) {
			deleted.push_back(net);
			for (auto e: net->endpoints) {
			}
			//delete net;
		}
	}
}
}
}
