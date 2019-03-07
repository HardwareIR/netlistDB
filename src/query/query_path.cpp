#include <netlistDB/query/query_path.h>


namespace netlistDB {
namespace query {

std::pair<QueryPath::path_t, bool> QueryPath::find_path(iNode & a, iNode & b) {
	path_t path;
	bool found = find_path(a, b, path);
	return {path, found};
}

// [TODO] tmp solution super sub-optimal
bool QueryPath::find_path(iNode & a, iNode & b, path_t & path) {
	path.push_back(&a);
	if (&a == &b)
		return true;

	bool found = false;
	a.forward([&](iNode & node) {
		bool not_in_path = std::find(path.begin(), path.end(), &node)
				== path.end();
		if (not_in_path) {
			found = find_path(node, b, path);

		}
		return found;
	});
	if (found)
		return true;
	path.pop_back();
	return false;
}

}
}
