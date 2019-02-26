#include <netlistDB/hw_type/hw_int.h>

namespace netlistDB {
namespace hw_type {

HwIntValue::HwIntValue(const HwInt & t, uint64_t value) :
		t(t), value(value), mask(t.all_mask) {
}

HwIntValue::HwIntValue(const HwInt & t, uint64_t value, uint64_t mask) :
		t(t), value(value), mask(t.all_mask) {
}

HwIntValue::HwIntValue(const HwInt & t, const aint_t & value,
		const aint_t & mask) :
		t(t), value(value), mask(t.all_mask) {
}

}
}
