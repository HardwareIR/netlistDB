#pragma once
#include "transformation.h"

namespace netlistDB {
namespace transform {

class TransformRemoveUseless: iTransformation {
public:
	/*
	 * Walk from the outputs of the netlit to all drivers of the outputs recursively
	 * The nodes which was not visited are not connected to any output and are useless then.
	 * @note Exact steps:
	 *     Walk the netlist,
	 *     discover what was used
	 *     collect what was not used
	 *     disconnect what was not used
	 *     delete what was not used.
	 *
	 * @return true if the netlist was modified
	 **/
	virtual bool apply(Netlist & ctx) override;

};

}
}
