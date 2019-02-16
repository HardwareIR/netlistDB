#pragma once

#include "../netlist.h"

namespace netlistDB {
namespace transform {

class iTransformation {
public:
	/*
	 * Apply transformation to netlist
	 *
	 * @return true if circuit was modified
	 **/
	virtual bool apply(Netlist & ctx) = 0;

	virtual ~iTransformation() {
	}
};

}
}
