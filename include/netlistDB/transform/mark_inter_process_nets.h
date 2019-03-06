#pragma once

#include <netlistDB/transform/transformation.h>

namespace netlistDB {
namespace transform {



class NETLISTDB_PUBLIC TransformMarkInterProcessNets: public iTransformation {
public:
	/*
	 * Walk all IO and outputs of the statements and set id.hidden flag to false
	 * */
	virtual bool apply(Netlist & ctx) override;
};

}
}
