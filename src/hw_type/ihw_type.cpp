#include <netlistDB/hw_type/ihw_type.h>

namespace netlistDB {
namespace hw_type {

iHwType_array::iHwType_array(const iHwType & elm_t, size_t size) :
		elm_t(elm_t), size(size) {
}

size_t iHwType_array::bit_length() const {
	return elm_t.bit_length() * size;
}

bool iHwType_array::operator==(const iHwType & other) const {
	if (this == &other)
		return true;

	auto a_t = dynamic_cast<const iHwType_array*>(&other);
	return a_t and size == a_t->size and elm_t == a_t->elm_t;
}

iHwType_array iHwType::operator[](size_t size) const {
	return iHwType_array(*this, size);
}

iHwType::~iHwType() {
}

}
}
