#include <netlistDB/bit_utils.h>

namespace netlistDB {
namespace bit_utils {

hw_type::aint_t mask(std::size_t len) {
	return (hw_type::aint_t(1) << len) - 1;
}

hw_type::aint_t select_bits(const hw_type::aint_t & val, std::size_t bitsStart,
		std::size_t bitsLen) {
	return (val >> bitsStart) & mask(bitsLen);
}

hw_type::aint_t set_bits(const hw_type::aint_t & val, std::size_t bitStart,
		std::size_t bitsLen, const hw_type::aint_t & newBits) {
	auto _mask = mask(bitsLen) << bitStart;
	auto _new_bits = ((newBits & _mask) << bitStart);
	return (val & ~_mask) | _new_bits;
}

}
}
