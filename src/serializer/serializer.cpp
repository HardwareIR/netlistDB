#include <netlistDB/serializer/serializer.h>
#include <netlistDB/hw_type/hw_int.h>

using namespace std;
using namespace netlistDB::hw_type;

namespace netlistDB {
namespace serializer {

void Serializer::serialize_value(const iHwTypeValue & val, ostream & str) {
	auto int_v = dynamic_cast<const HwInt::value_type*>(&val);
	if (int_v) {
		serialize_value(*int_v, str);
		return;
	}
	throw runtime_error(string("not implemented") + __PRETTY_FUNCTION__);
}

void Serializer::serialize_stm(const Statement & stm, ostream & str) {
	auto a = dynamic_cast<const Assignment*>(&stm);
	if (a) {
		serialize_stm(*a, str);
		return;
	}
	auto i = dynamic_cast<const IfStatement*>(&stm);
	if (i) {
		serialize_stm(*i, str);
		return;
	}
	throw runtime_error(string("Not implemented ") + __PRETTY_FUNCTION__);
}

void Serializer::serialize_operand(const Net & operand,
		const FunctionCall & oper, bool expr_requires_braces, ostream & str) {
	if (operand.id.hidden and operand.val == nullptr) {
		// not id or value
		if (operand.drivers.size() == 1) {
			auto d = dynamic_cast<FunctionCall*>(operand.drivers[0]);
			if (d) {
				auto prec = get_operator_precedence();
				auto p = prec.find(&oper.fn);
				auto child_p = prec.find(&oper.fn);
				assert(p != prec.end());
				assert(child_p != prec.end());

				if (child_p->second <= p->second) {
					str << "(";
					serialize_net_usage(operand, str);
					str << ")";
					return;
				}
			}
		}
	}
	serialize_net_usage(operand, str);
}

}
}
