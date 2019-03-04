#pragma once

#include <netlistDB/hw_type/ihw_type.h>
#include <boost/multiprecision/cpp_int.hpp>
//#include <boost/multiprecision/gmp.hpp>
#include <netlistDB/hw_type/ihw_type_value.h>

namespace netlistDB {
namespace hw_type {

class HwInt;

/* Container of the value for hardware integer type
 **/
class HwIntValue: public iHwTypeValue {
public:
	using aint_t = boost::multiprecision::cpp_int;

	// backward reference to type
	// (to keep track of the sign and size, ...)
	const HwInt & t;

	// bits which does not have mask set are 0
	// the value is trimmed to size of the type
	aint_t value;
	// mask of size of the type, if the bit is 0 it means value
	// is undefined
	aint_t mask;

	HwIntValue(const HwInt & t, uint64_t val);
	HwIntValue(const HwInt & t, uint64_t val, uint64_t mask);
	HwIntValue(const HwInt & t, const aint_t & value, const aint_t & mask);
};

/*
 * Basic hardware integer type with specified size and sign
 * */
class HwInt: public iHwType {
	// size of value in bits
	const size_t _M_bit_length;

public:
	// value class used by this type class
	using value_type = HwIntValue;

	// the mask value for the type of this size
	const boost::multiprecision::cpp_int all_mask;

	// true if the value is signed integer
	const size_t is_signed;
	// if false vector of size 1 is converted to scalar in HDL
	// (affect only the representation in final HDL)
	const bool has_to_be_vector;

	HwInt(size_t bit_length, bool is_signed = false, bool has_to_be_vector = false);

	virtual size_t bit_length() const override;
	virtual bool operator==(const iHwType & other) const override;

	~HwInt();
};

}
}
