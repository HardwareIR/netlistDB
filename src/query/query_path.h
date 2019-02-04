#pragma once

#include <functional>
#include "backtrack_context.h"

namespace netlistDB {
namespace query {
class QueryPath {
public:
	using path_t = std::vector<iNode*>;

	static std::pair<path_t, bool> find_path(iNode & a, iNode & b);
	static bool find_path(iNode & a, iNode & b, path_t & path);
};

}
}
