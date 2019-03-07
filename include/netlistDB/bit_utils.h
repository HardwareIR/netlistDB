#pragma once

#include <netlistDB/common.h>
#include <netlistDB/hw_type/hw_int.h>

namespace netlistDB {
namespace bit_utils {

hw_type::aint_t mask(std::size_t len);

hw_type::aint_t select_bits(const hw_type::aint_t & val, std::size_t bitsStart,
		std::size_t bitsLen);

hw_type::aint_t set_bits(const hw_type::aint_t & val, std::size_t bitStart,
		std::size_t bitsLen, const hw_type::aint_t & newBits);

}
}
