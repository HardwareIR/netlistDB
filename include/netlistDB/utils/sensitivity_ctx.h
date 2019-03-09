#pragma once

#include <netlistDB/inode.h>
#include <netlistDB/utils/ordered_set.h>

namespace netlistDB {
namespace utils {

/*
 * Sensitivity list used for resolution of sensitivity for hw statement instances
 *
 * @ivar contains_event_dep: true if this contains event dependent
 *    sensitivity
 */

class NETLISTDB_PUBLIC SensitivityCtx: public utils::OrderedSet<iNode*> {
public:
	bool contains_event_dep;
	SensitivityCtx() :
			contains_event_dep(false) {
	}

	template<typename iterable>
	void extend(const iterable & other) {
		_extend<iterable>(other);
	}

	void clear() {
		utils::OrderedSet<iNode*>::clear();
		contains_event_dep = false;
	}
private:
	template<typename iterable>
	void _extend(const iterable & other) {
		utils::OrderedSet<iNode*>::extend(other);
	}
	void _extend(const SensitivityCtx & other) {
		utils::OrderedSet<iNode*>::extend(other);
		contains_event_dep |= other.contains_event_dep;
	}

};

}
}
