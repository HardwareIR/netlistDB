#include <netlistDB/bit_utils.h>

namespace netlistDB {
namespace bit_utils {

boost::multiprecision::cpp_int mask(std::size_t len) {
	return (boost::multiprecision::cpp_int(1) << len) - 1;
}

boost::multiprecision::cpp_int select_bits(
		boost::multiprecision::cpp_int val, std::size_t bitsStart,
		std::size_t bitsLen) {
	val >>= bitsStart;
	return val & mask(bitsLen);
}

boost::multiprecision::cpp_int set_bits(
		boost::multiprecision::cpp_int val, std::size_t bitStart,
		std::size_t bitsLen, boost::multiprecision::cpp_int newBits) {
	boost::multiprecision::cpp_int _mask = mask(bitsLen);
	newBits &= _mask;

	_mask <<= bitStart;
	newBits <<= bitStart;

	return (val & ~_mask) | newBits;
}

}
}
