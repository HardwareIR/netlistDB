#pragma once

#include "ihw_type.h"
#include "../netlist.h"

namespace netlistDB {
namespace hw_type {

class HwInt: public iHwType {

	size_t _M_bit_length;
	size_t _M_is_signed;

public:
	// if false vector of size 1 is converted to scalar in HDL
	// (affect only the representation in final HDL)
	const bool has_to_be_vector;

	virtual size_t bit_length() override;

	HwInt(size_t bit_length, bool is_signed, bool has_to_be_vector = false);

	Net & operator() (uint64_t val);
	Net & operator() (int64_t val);

	~HwInt();
};

}
}
