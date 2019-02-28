#include <netlistDB/serializer/serializer.h>

namespace netlistDB {
namespace serializer {

void Serializer::serialize(const Statement & stm, std::ostream & str) {
	auto a = dynamic_cast<const Assignment*>(&stm);
	if (a) {
		serialize(*a, str);
		return;
	}
	auto i = dynamic_cast<const IfStatement*>(&stm);
	if (i) {
		serialize(*i, str);
		return;
	}
	throw std::runtime_error("void Serializer::serialize(const Statement & stm, std::ostream & str): Unknon type of statement");

}

}
}
