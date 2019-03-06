#pragma once

#include <set>

#include <netlistDB/netlist.h>

namespace netlistDB {
namespace query {

/*
 * Discover sensitivity in expression hidden behind
 * @note the query is bounded by other statements and does not pass them
 * */
class NETLISTDB_PUBLIC ExprSensitivityProbe {
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
