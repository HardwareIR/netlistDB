#pragma once

#include <stddef.h>
#include <stdexcept>

namespace netlistDB {
namespace hw_type {


class NETLISTDB_PUBLIC iHwTypeValue {
public:
	virtual bool operator==(const iHwTypeValue & other) const {
		throw std::runtime_error("undefined operator==");
	}
	virtual ~iHwTypeValue() {
	}
};

}
}
