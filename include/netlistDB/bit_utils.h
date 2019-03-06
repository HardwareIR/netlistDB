#pragma once

#include <boost/multiprecision/cpp_int.hpp>
#include <netlistDB/common.h>

namespace netlistDB {
namespace bit_utils {

boost::multiprecision::cpp_int mask(std::size_t len);

boost::multiprecision::cpp_int select_bits(boost::multiprecision::cpp_int val,
		std::size_t bitsStart, std::size_t bitsLen);

boost::multiprecision::cpp_int set_bits(boost::multiprecision::cpp_int val,
		std::size_t bitStart, std::size_t bitsLen,
		boost::multiprecision::cpp_int newBits);

}
}
