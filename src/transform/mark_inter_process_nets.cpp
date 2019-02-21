#include <netlistDB/transform/mark_inter_process_nets.h>

namespace netlistDB {
namespace transform {

bool TransformMarkInterProcessNets::apply(Netlist & ctx) {
	bool any_updated = false;
	for (auto n : ctx.nodes) {
		auto net = dynamic_cast<Net*>(n);
		if (net) {
			if (net->direction != Direction::DIR_UNKNOWN) {
				net->id.hidden = false;
				any_updated = true;
			}
			continue;
		}
		auto stm = dynamic_cast<Statement*>(n);
		if (stm) {
			if (stm->parent == nullptr) {
				for (auto o : stm->_outputs) {
					o->id.hidden = false;
					any_updated = true;
				}
			}
		}
	}
	return any_updated;

}

}
}
