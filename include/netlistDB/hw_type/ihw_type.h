#pragma once

#include <stddef.h>
#include <netlistDB/common.h>

namespace netlistDB {
namespace hw_type {

class iHwType_array;

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
	virtual iHwType_array operator[](size_t size) const;
	virtual ~iHwType();

};

class NETLISTDB_PUBLIC iHwType_array: public iHwType {
public:
	const iHwType & elm_t;
	const size_t size;

	iHwType_array(const iHwType & elm_t, size_t size);
	virtual size_t bit_length() const;
	virtual bool operator==(const iHwType & other) const;
};


}
}
