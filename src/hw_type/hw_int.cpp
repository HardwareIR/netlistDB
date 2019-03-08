#include <netlistDB/hw_type/hw_int.h>
#include <netlistDB/bit_utils.h>

namespace netlistDB {
namespace hw_type {

size_t HwInt::bit_length() const {
	return _M_bit_length;
}

bool HwInt::operator==(const iHwType & _other) const {
	if (this == &_other)
		return true;

	auto other = dynamic_cast<const HwInt*>(&_other);
	if (other == nullptr)
		return false;

	if (bit_length() == other->bit_length() and is_signed == other->is_signed) {
		return bit_length() > 1 or has_to_be_vector == other->has_to_be_vector;
	}
	return false;
}

HwInt::HwInt(size_t bit_length, bool is_signed, bool has_to_be_vector) :
		_M_bit_length(bit_length), all_mask(bit_utils::mask(bit_length)), is_signed(
				is_signed), has_to_be_vector(has_to_be_vector) {
}

Net & HwInt::operator()(Netlist & ctx, const aint_t & val) {
	Net & n = ctx.sig("const_", *this);
	n.val = new value_type(*this, val, all_mask);
	return n;
}

Net & HwInt::operator()(Netlist & ctx, const aint_t & val,
		const aint_t & mask) {
	Net & n = ctx.sig("const_", *this);
	n.val = new value_type(*this, val, mask);
	return n;
}

HwInt::~HwInt() {
}

}
}
