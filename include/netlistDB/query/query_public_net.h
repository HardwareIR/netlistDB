#pragma once

#include <functional>

#include <netlistDB/netlist.h>

namespace netlistDB {
namespace query {

/*
 * Walk the net which are not marked as a hidden in expression
 * This nets represents nets which are connected between hdl process
 * instances and IO.
 **/

class QueryPublicNet {
public:

	static void walk(Net & n, std::set<Net*> seen,
			std::function<void(Net & n)> fn);
	static void walk(FunctionCall & fncall, std::set<Net*> seen,
			std::function<void(Net & n)> fn);

};

}
}
