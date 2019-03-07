#include <netlistDB/bit_utils.h>

namespace netlistDB {
namespace bit_utils {

hw_type::aint_t mask(std::size_t len) {
	return (hw_type::aint_t(1) << len) - 1;
}

hw_type::aint_t select_bits(const hw_type::aint_t & val, std::size_t bitsStart,
		std::size_t bitsLen) {
	val >>= bitsStart;
	return val & mask(bitsLen);
}

hw_type::aint_t set_bits(const hw_type::aint_t & val, std::size_t bitStart,
		std::size_t bitsLen, const hw_type::aint_t & newBits) {
	hw_type::aint_t _mask = mask(bitsLen);
	newBits &= _mask;

	_mask <<= bitStart;
	newBits <<= bitStart;

	return (val & ~_mask) | newBits;
}

}
}
