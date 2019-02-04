#include "query_path.h"


namespace netlistDB {
namespace query {

std::pair<QueryPath::path_t, bool> QueryPath::find_path(iNode & a, iNode & b) {
	path_t path;
	bool found = find_path(a, b, path);
	return {path, found};
}

bool QueryPath::find_path(iNode & a, iNode & b, path_t & path) {
	path.push_back(&a);
	if (&a == &b)
		return true;

	for (auto node : a.forward()) {
		bool not_in_path = std::find(path.begin(), path.end(), node)
				== path.end();
		if (not_in_path) {
			auto found = find_path(*node, b, path);
			if (found)
				return true;
		}
	}
	path.pop_back();
	return false;
}

}
}
