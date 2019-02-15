#pragma once

#include "../netlist.h"

namespace netlistDB {
namespace transform {

class iTransformation {
public:
	/*
	 * Apply transformation to netlist
	 *
	 * @param thread_cnt the number of threads to use if 0 the optimal number will be used
	 * @return true if circuit was modified
	 **/
	virtual bool apply(Netlist & ctx, size_t thread_cnt = 0) = 0;

	virtual ~iTransformation() {
	}
};

}
}
