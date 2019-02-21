#pragma once

#include <netlistDB/transform/transformation.h>

namespace netlistDB {
namespace transform {



class TransformMarkInterProcessNets: public iTransformation {
public:
	/*
	 * Walk all IO and outputs of the statements and set id.hidden flag to false
	 * */
	virtual bool apply(Netlist & ctx) override;
};

}
}
