#include "hw_int.h"

namespace netlistDB {
namespace hw_type {

size_t HwInt::bit_length() {
	return _M_bit_length;
}

HwInt::HwInt(size_t bit_length, bool is_signed, bool has_to_be_vector) :
		_M_bit_length(bit_length), _M_is_signed(is_signed), has_to_be_vector(
				has_to_be_vector) {
}

Net & HwInt::operator()(uint64_t val) {
}

Net & HwInt::operator()(int64_t val) {
}

HwInt::~HwInt() {
}

}
}
