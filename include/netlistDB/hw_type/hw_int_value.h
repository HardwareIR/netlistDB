#pragma once

#include <boost/multiprecision/cpp_int.hpp>
//#include <boost/multiprecision/gmp.hpp>
#include <netlistDB/hw_type/ihw_type_value.h>
#include <netlistDB/hw_type/hw_int.h>

namespace netlistDB {
namespace hw_type {

/* Container of the value for integer type
 **/
class HwIntValue: public iHwTypeValue {
public:
	HwIntValue(const HwInt & t, uint64_t val);
	HwIntValue(const HwInt & t, uint64_t val, uint64_t mask);
	HwIntValue(const HwInt & t, const boost::multiprecision::cpp_int & value,
			const boost::multiprecision::cpp_int & mask);

	// backward reference to type
	// (to keep track of the sign and size, ...)
	const HwInt & t;

	// bits which does not have mask set are 0
	// the value is trimmed to size of the type
	boost::multiprecision::cpp_int value;
	// mask of size of the type, if the bit is 0 it means value
	// is undefined
	boost::multiprecision::cpp_int mask;
};

}
}
