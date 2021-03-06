#include <netlistDB/netlist.h>

namespace netlistDB {

ComponentMap::ComponentMap(Netlist & parent, std::shared_ptr<Netlist> component) :
		parent(parent), component(component) {
}

void ComponentMap::connect(Net & parent_net, Net & component_port) {
	assert(&parent_net.ctx == &parent);
	assert(&component_port.ctx == component.get());
	assert(parent_to_child.find(&parent_net) == parent_to_child.end());
	assert(child_to_parent.find(&component_port) == child_to_parent.end());

	parent_to_child[&parent_net] = &component_port;
	child_to_parent[&component_port] = &parent_net;

	auto d = component_port.direction;
	if (d == Direction::DIR_OUT or d == Direction::DIR_INOUT) {
		parent_net.drivers.push_back(this);
		component_port.endpoints.push_back(this);
	} else if (d == Direction::DIR_IN or d == Direction::DIR_INOUT) {
		parent_net.endpoints.push_back(this);
		component_port.drivers.push_back(this);
	}
}

void ComponentMap::forward(const predicate_t & fn) {
	for (auto p : child_to_parent) {
		auto d = p.first->direction;
		if (d == Direction::DIR_OUT or d == Direction::DIR_INOUT)
			if (fn(*p.first))
				return;
	}
}

void ComponentMap::backward(const predicate_t & fn) {
	for (auto p : child_to_parent) {
		auto d = p.first->direction;
		if (d == Direction::DIR_IN or d == Direction::DIR_INOUT)
			if (fn(*p.first))
				return;
	}

}

}
