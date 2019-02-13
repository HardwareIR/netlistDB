#include "hw_int.h"

namespace netlistDB {
namespace hw_type {

size_t HwInt::bit_length() const {
	return _M_bit_length;
}

bool HwInt::operator==(const iHwType & _other) const {
	auto other = dynamic_cast<const HwInt*>(&_other);
	if (other == nullptr)
		return false;

	if (bit_length() == other->bit_length() and is_signed == other->is_signed) {
		return bit_length() > 1 or has_to_be_vector == other->has_to_be_vector;
	}
	return false;
}

HwInt::HwInt(size_t bit_length, bool is_signed, bool has_to_be_vector) :
		_M_bit_length(bit_length), is_signed(is_signed), has_to_be_vector(
				has_to_be_vector) {
}

HwInt::~HwInt() {
}

}
}
