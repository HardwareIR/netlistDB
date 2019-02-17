#pragma once

#include <string>

namespace netlistDB {

/**
 * Container for the id od the variable
 *
 **/
class VarId {
public:
	std::string name;
	bool hidden;

	VarId(const std::string & name) :
			name(name), hidden(true) {
	}
};

}
