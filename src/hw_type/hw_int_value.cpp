#include "hw_int_value.h"

namespace netlistDB {
namespace hw_type {

HwIntValue::HwIntValue(const HwInt & t, uint64_t value) :
		t(t), value(value), mask(t.all_mask) {
}

HwIntValue::HwIntValue(const HwInt & t, uint64_t value, uint64_t mask) :
		t(t), value(value), mask(t.all_mask) {
}

HwIntValue::HwIntValue(const HwInt & t,
		const boost::multiprecision::cpp_int & value,
		const boost::multiprecision::cpp_int & mask) :
		t(t), value(value), mask(t.all_mask) {
}

}
}
