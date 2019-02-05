#include "netlist.h"

using namespace netlistDB;

Assignment::Assignment(Net & dst, Net & src) : dst(dst), src(src) {
	dst.ctx.register_node(*this);
	dst.drivers.push_back(this);
	src.endpoints.push_back(this);
}

Assignment::Assignment(Net & dst, std::initializer_list<Net*> dst_index,
		Net & src) : dst(dst), dst_index(dst_index), src(
				src) {
	dst.ctx.register_node(*this);
	dst.drivers.push_back(this);
	for (auto i : dst_index) {
		i->endpoints.push_back(this);
	}
	src.endpoints.push_back(this);
}

iNode::iterator Assignment::forward() {
	iNode::iterator it;
	it.push_back(&dst);
	return it;
}

iNode::iterator Assignment::backward() {
	iNode::iterator it;
	for (auto i : dst_index)
		it.push_back(i);
	it.push_back(&src);
	return it;
}
