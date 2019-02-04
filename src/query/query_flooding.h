#pragma once

#include "netlist.h"

namespace netlistDB {
namespace query {

class QueryFlood {
	// function which select where to flood
	// <to driver, to endpoints>
	using flooding_predicate = std::function<std::pair<bool, bool>(iNode &)>;

	void apply(Netlist & netlist, flooding_predicate predicate);
};

}
}
