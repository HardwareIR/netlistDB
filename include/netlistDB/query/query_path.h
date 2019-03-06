#pragma once

#include <functional>
#include <netlistDB/netlist.h>

namespace netlistDB {
namespace query {

/*
 * Find the path between two nodes
 * */
class NETLISTDB_PUBLIC QueryPath {
public:
	using path_t = std::vector<iNode*>;

	static std::pair<path_t, bool> find_path(iNode & a, iNode & b);
	static bool find_path(iNode & a, iNode & b, path_t & path);
};

}
}
