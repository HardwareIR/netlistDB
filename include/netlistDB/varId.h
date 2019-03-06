#pragma once

#include <string>

namespace netlistDB {

/**
 * Container for the id of the variable
 *
 **/
class NETLISTDB_PUBLIC VarId {
public:
	std::string name;
	bool hidden;

	VarId(const std::string & name) :
			name(name), hidden(true) {
	}
};

}
