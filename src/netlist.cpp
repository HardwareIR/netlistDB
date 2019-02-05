#include "netlist.h"
#include "statemen_if.h"

using namespace netlistDB;

Netlist::Netlist(const std::string & name) :
		name(name), obj_seq_num(0) {
}

void Netlist::register_node(iNode & n) {
	n.index = nodes.size();
	nodes.push_back(&n);
}

void Netlist::register_node(Net & n) {
	nets.insert(&n);
	register_node(static_cast<iNode&>(n));
}

void Netlist::unregister_node(iNode & n) {
	nodes[n.index] = nullptr;
}

void Netlist::unregister_node(Net & n) {
	nets.erase(&n);
	unregister_node(static_cast<iNode&>(n));
}

Net & Netlist::sig_in(const std::string & name) {
	auto s = new Net(*this, name, DIR_IN);
	nets.insert(s);
	return *s;
}

Net & Netlist::sig_out(const std::string & name) {
	auto s = new Net(*this, name, DIR_OUT);
	return *s;
}

Net & Netlist::sig() {
	return sig("");
}

Net & Netlist::sig(const std::string & name) {
	auto s = new Net(*this, name, DIR_UNKNOWN);
	return *s;
}

Netlist::~Netlist() {
	// [TODO] allocating the memory during the deallocation
	// can deadlock when deallocating because we run out of the memory
	for (auto o : nodes) {
		if (o != nullptr)
			delete o;
	}
}
