#pragma once
#include <stddef.h>

namespace netlistDB {
namespace hw_type {

/*
 * Interface class for hardware type
 * The hardware type use used to store informations about type for nets and values
 * in generated circuit.
 **/
class iHwType {
public:
	// @return number of bits required for representation of value for this type
	virtual size_t bit_length() = 0;
	virtual ~iHwType();
};

}
}
