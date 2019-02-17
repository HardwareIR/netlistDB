#include "../netlist.h"
#include <set>

namespace netlistDB {
namespace query {

/*
 * Discover sensitivity in expression hidden behind
 * @note the query is bounded by other statements and does not pass them
 * */
class ExprSensitivityProbe {
public:
	static void apply(FunctionCall & fn_call,
			std::set<Net*> & casual_sensitivity, std::set<iNode*> & seen,
			SensitivityCtx & ctx);

	static void apply(Net & net, std::set<Net*> & casual_sensitivity,
			std::set<iNode*> & seen, SensitivityCtx & ctx);

	static void apply(Net & net, std::set<iNode*> & seen, SensitivityCtx & ctx);
};

}
}
