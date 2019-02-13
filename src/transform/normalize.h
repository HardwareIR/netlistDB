#pragma once
#include "transformation.h"

namespace netlistDB {
namespace transform {

class TransformNormalize: iTransformation {
public:
	/*
	 * Apply transformation to netlist
	 *
	 * @return true if circuit was modified
	 **/
	virtual bool apply(Netlist & ctx) override;
};

}
}
