#include "netlist.h"

using namespace netlistDB;

Assignment::Assignment(Net & dst, Net & src) :
		Statement(), dst(dst), src(src) {

}

Assignment::Assignment(Net & dst, std::initializer_list<Net*> dst_index,
		Net & src) :
		dst(dst), dst_index(dst_index), src(src) {
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
