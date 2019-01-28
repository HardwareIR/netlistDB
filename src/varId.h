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

	VarId(const std::string & name) :
			name(name) {
	}
};

}
