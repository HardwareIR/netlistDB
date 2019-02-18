#include "query_public_net.h"

#include <assert.h>

using namespace std;

namespace netlistDB {
namespace query {

void QueryPublicNet::walk(Net & n, std::set<Net*> seen,
		std::function<void(Net & n)> fn) {
	seen.insert(&n);
	if (not n.id.hidden) {
		fn(n);
		return;
	}

	assert(n.drivers.size() > 0
			or n.nop_val != nullptr
			or n.val != nullptr);

	for (auto d : n.drivers) {
		auto _d = dynamic_cast<FunctionCall*>(d);
		if (_d)
			walk(*_d, seen, fn);
	}
}
void QueryPublicNet::walk(FunctionCall & fncall, std::set<Net*> seen,
		std::function<void(Net & n)> fn) {
	for (Net * arg : fncall.args) {
		auto f = seen.find(arg);
		if (f == seen.end())
			walk(*arg, seen, fn);
	}
}

}
}
