#pragma once

#include <stddef.h>
#include <boost/multiprecision/cpp_int.hpp>
#include <netlistDB/common.h>

namespace netlistDB {
namespace hw_type {

/*
 * Interface class for hardware type
 * The hardware type use used to store informations about type for nets and values
 * in generated circuit.
 **/
class NETLISTDB_PUBLIC iHwType {
public:
	// @return number of bits required for representation of value for this type
	virtual size_t bit_length() const = 0;
	virtual bool operator==(const iHwType & other) const = 0;
	virtual ~iHwType();

};

}
}
