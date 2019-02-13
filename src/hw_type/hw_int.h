#pragma once

#include "ihw_type.h"
#include "../netlist.h"

namespace netlistDB {
namespace hw_type {

class HwInt: public iHwType {
	// size of value in bits
	const size_t _M_bit_length;

public:
	// true if the value is signed integer
	const size_t is_signed;
	// if false vector of size 1 is converted to scalar in HDL
	// (affect only the representation in final HDL)
	const bool has_to_be_vector;

	virtual size_t bit_length() const override;
	virtual bool operator==(const iHwType & other) const override;
	HwInt(size_t bit_length, bool is_signed, bool has_to_be_vector = false);

	~HwInt();
};

}
}
