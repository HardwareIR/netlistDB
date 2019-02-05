#pragma once
#include "transformation.h"

namespace netlistDB {
namespace transform {

class TransformRemoveUseless: iTransformation {
public:
	/*
	 * Walk from the outputs of the netlit to all drivers of the outputs recursively
	 * The nodes which was not visited are not connected to any output and are useless then.
	 *
	 * @return true if the netlist was modified
	 **/
	virtual bool apply(Netlist & ctx) override;

};

}
}
