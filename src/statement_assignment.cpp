#include "statement_assignment.h"

namespace netlistDB {

void init_iterators(Assignment * a) {
	a->forward.push_back(reinterpret_cast<iNode*>(&a->dst));
	a->backward.push_back(reinterpret_cast<iNode*>(&a->src));
	a->backward.push_back(reinterpret_cast<std::vector<iNode*>*>(&a->dst_index));
}

Assignment::Assignment(Net & dst, Net & src) :
		dst(dst), src(src) {
	dst.ctx.register_node(*this);
	dst.drivers.push_back(this);
	src.endpoints.push_back(this);
	init_iterators(this);
}

Assignment::Assignment(Net & dst, std::initializer_list<Net*> dst_index,
		Net & src) :
		dst(dst), dst_index(dst_index), src(src) {
	dst.ctx.register_node(*this);
	dst.drivers.push_back(this);
	for (auto i : dst_index) {
		i->endpoints.push_back(this);
	}
	src.endpoints.push_back(this);
	init_iterators(this);
}

}
