#pragma once
#include <string>
#include "netlist.h"
#include "../statement_hwprocess.h"

namespace netlistDB {
namespace transform {

class TransformStatementToHwProcess {

	void apply(const std::vector<Statement*> & statements,
			std::vector<HwProcess*> & res, bool try_solve_comb_loops);

};

}
}
