#pragma once
#include <stddef.h>
#include <exception>

namespace netlistDB {
namespace hw_type {

class iHwTypeValue {
public:
	virtual ~iHwTypeValue() {
	}
	virtual bool operator==(const iHwTypeValue & other) const {
		throw std::runtime_error("undefined operator==");
	}
};

}
}
