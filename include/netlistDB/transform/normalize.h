#pragma once

#include <netlistDB/transform/transformation.h>

namespace netlistDB {
namespace transform {

class NETLISTDB_PUBLIC TransformNormalize: iTransformation {
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
