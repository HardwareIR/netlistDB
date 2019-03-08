#pragma once

#include <netlistDB/netlist.h>
#include <netlistDB/hw_type/ihw_type.h>
#include <netlistDB/hw_type/ihw_type_value.h>
#include <netlistDB/hw_type/aint.h>


namespace netlistDB {
namespace hw_type {


class NETLISTDB_PUBLIC HwInt;

/*
 * */
/* Container of the value for hardware integer type
 **/
class NETLISTDB_PUBLIC HwIntValue: public iHwTypeValue {
public:

	// backward reference to type
	// (to keep track of the sign and size, ...)
	const HwInt & t;

	// bits which does not have mask set are 0
	// the value is trimmed to size of the type
	aint_t value;
	// mask of size of the type, if the bit is 0 it means value
	// is undefined
	aint_t mask;

	HwIntValue(const HwInt & t, const aint_t & value);
	HwIntValue(const HwInt & t, uint64_t val);
	HwIntValue(const HwInt & t, int64_t val);
	HwIntValue(const HwInt & t, unsigned val);
	HwIntValue(const HwInt & t, int val);

	HwIntValue(const HwInt & t, const aint_t & value, const aint_t & mask);
	HwIntValue(const HwInt & t, uint64_t val, aint_t mask);
	HwIntValue(const HwInt & t, int64_t val, aint_t mask);
	HwIntValue(const HwInt & t, unsigned val, aint_t mask);
	HwIntValue(const HwInt & t, int val, aint_t mask);
};

/*
 * Basic hardware integer type with specified size and sign
 * */
class NETLISTDB_PUBLIC HwInt: public iHwType {
	// size of value in bits
	const size_t _M_bit_length;

public:
	// value class used by this type class
	using value_type = HwIntValue;

	// the mask value for the type of this size
	const aint_t all_mask;

	// true if the value is signed integer
	const size_t is_signed;
	// if false vector of size 1 is converted to scalar in HDL
	// (affect only the representation in final HDL)
	const bool has_to_be_vector;

	HwInt(size_t bit_length, bool is_signed = false, bool has_to_be_vector = false);

	virtual size_t bit_length() const override;
	virtual bool operator==(const iHwType & other) const override;

	// create constant net of this type in specified netlist
	Net & operator()(Netlist & ctx, const aint_t & val);
	Net & operator()(Netlist & ctx, const aint_t & val, const aint_t & mask);
	template<typename T>
	Net & operator()(Netlist & ctx, T val) {
		Net & n = ctx.sig("const_", *this);
		n.val = new value_type(*this, val, all_mask);
		return n;
	}
	template<typename T>
	Net & operator()(Netlist & ctx, T val, aint_t mask) {
		Net & n = ctx.sig("const_", *this);
		n.val = new value_type(*this, val, mask);
		return n;
	}
	~HwInt();
};

}
}
