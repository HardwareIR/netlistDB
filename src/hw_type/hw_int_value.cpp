#include <netlistDB/hw_type/hw_int.h>

namespace netlistDB {
namespace hw_type {

HwIntValue::HwIntValue(const HwInt & t, const aint_t & value) :
		t(t), value(value), mask(t.all_mask) {
}
HwIntValue::HwIntValue(const HwInt & t, uint64_t value) :
		t(t), value(value), mask(t.all_mask) {
}
HwIntValue::HwIntValue(const HwInt & t, int64_t value) :
		t(t), value(value), mask(t.all_mask) {
}
HwIntValue::HwIntValue(const HwInt & t, unsigned value) :
		t(t), value(value), mask(t.all_mask) {
}
HwIntValue::HwIntValue(const HwInt & t, int value) :
		t(t), value(value), mask(t.all_mask) {
}

HwIntValue::HwIntValue(const HwInt & t, const aint_t & value,
		const aint_t & mask) :
		t(t), value(value), mask(mask) {
	assert(mask <= t.all_mask);
}
HwIntValue::HwIntValue(const HwInt & t, uint64_t value, aint_t mask) :
		t(t), value(value), mask(mask) {
	assert(aint_t(mask) <= t.all_mask);
}
HwIntValue::HwIntValue(const HwInt & t, int64_t value, aint_t mask) :
		t(t), value(value), mask(mask) {
	assert(aint_t(mask) <= t.all_mask);
}
HwIntValue::HwIntValue(const HwInt & t, unsigned value, aint_t mask) :
		t(t), value(value), mask(mask) {
	assert(aint_t(mask) <= t.all_mask);
}
HwIntValue::HwIntValue(const HwInt & t, int value, aint_t mask) :
		t(t), value(value), mask(mask) {
	assert(aint_t(mask) <= t.all_mask);
}

}
}
