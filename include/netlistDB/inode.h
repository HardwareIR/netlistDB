#pragma once

#include <functional>
#include <netlistDB/utils/chained_iterator.h>

namespace netlistDB {

/**
 * Interface for nodes in database
 *
 **/
class iNode {
public:
	// sequential number used as a id during serialization
	// and as a index in main list of iNode instances in Netlist.nodes
	size_t index;

	using iterator = utils::ChainedSequence<iNode*>;
	using predicate_t = std::function<bool(iNode*)>;

	// iterator of endpoints for Net or outputs for statement or result for expression
	iterator forward;
	// iterator of drivers for net or inputs for statement or args for expression
	iterator backward;

	virtual ~iNode() {
	}
};

}
