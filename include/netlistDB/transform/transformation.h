#pragma once

#include <netlistDB/netlist.h>

namespace netlistDB {
namespace transform {

class NETLISTDB_PUBLIC iTransformation {
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
