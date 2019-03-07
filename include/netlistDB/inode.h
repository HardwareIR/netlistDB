#pragma once

#include <functional>

#include <netlistDB/common.h>

namespace netlistDB {

/**
 * Interface for nodes in database
 *
 **/
class NETLISTDB_PUBLIC iNode {
public:
	// sequential number used as a id during serialization
	// and as a index in main list of iNode instances in Netlist.nodes
	size_t index;

	using predicate_t = std::function<bool(iNode&)>;

	// iterator of endpoints for Net or outputs for statement or result for expression
	virtual void forward(const predicate_t & fn) = 0;
	// iterator of drivers for net or inputs for statement or args for expression
	virtual void backward(const predicate_t & fn) = 0;

	virtual ~iNode() {
	}
};

}
