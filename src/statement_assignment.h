#pragma once
#include "netlist.h"

namespace netlistDB {
class Assignment: public Statement {
public:
	Net & dst;
	std::vector<Net*> dst_index;
	Net & src;

	Assignment(const Assignment & other) = delete;
	Assignment(Net & dst, Net & src);
	Assignment(Net & dst, std::initializer_list<Net*> dst_index, Net & src);
	virtual iNode::iterator forward() override;
	virtual iNode::iterator backward() override;
};

}
