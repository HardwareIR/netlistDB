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
	throw runtime_error(
			string("not implemented") + std::string(__FILE__) + ":"
					+ std::to_string(__LINE__));
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
	throw runtime_error(
			string("Not implemented ") + std::string(__FILE__) + ":"
					+ std::to_string(__LINE__));
}

int Serializer::precedence_of_expr(const Net & n) {
	// not id or value
	if (not n.id.hidden or n.val != nullptr or n.drivers.size() != 1)
		return -1;
	auto d = dynamic_cast<const FunctionCall*>(n.drivers[0]);
	if (not d) {
		// the signal will not stay hidden
		return -1;
	}
	auto prec = get_operator_precedence();
	auto p = prec.find(&d->fn);
	assert(p != prec.end());
	return p->second;
}

void Serializer::serialize_operand(const Net & operand,
		const FunctionCall & oper, bool expr_requires_brackets, bool cancel_brackets, ostream & str) {

	bool use_brackets = false;
	if (not cancel_brackets) {
		// resolve if the brackets are required
		auto precedence_my = precedence_of_expr(operand);
		if (precedence_my >= 0) { // if this is expression
			if (expr_requires_brackets) {
				use_brackets = true;
			} else {
				auto prec = get_operator_precedence();
				auto p = prec.find(&oper.fn);
				assert(p != prec.end());
				auto precedence_parent = p->second;

				Net * right = nullptr;
				Net * left = nullptr;
				switch (oper.args.size()) {
				case 1:
					break;
				case 2:
					if (oper.args[0] == &operand) {
						right = oper.args[1];
					} else {
						left = oper.args[0];
					}
					break;
				default:
					throw runtime_error(
							string("Not implemented ") + std::string(__FILE__) + ":"
									+ std::to_string(__LINE__));
				}
				if (left) { // "operand" is right
					// same precedence -> brackets on right if it is expression
					// a + (b + c)
					// a + b + c = (a + b) + c
					// right with lower precedence -> brackets for right not required
					// a + b * c = a + (b * c)
					// right with higher precedence -> brackets for right
					// a * (b + c)
					if (precedence_my >= precedence_parent) {
						use_brackets = true;
					}
				} else {
					// "operand" is left
					if (precedence_my == precedence_parent) {
						if (precedence_of_expr(*right) == precedence_my) {
							// right and left with same precedence -> brackets on both sides
							// (a + b) + (c + d)
							use_brackets = true;
						}
					} else if (precedence_my > precedence_parent) {
						// left with higher precedence -> brackets for left
						// (a + b) * c
						// a + b + c + d = (a + b) + c + d = ((a + b) + c) + d
						use_brackets = true;
					}
				}
			}
		}
	}
	if (use_brackets)
		str << "(";

	serialize_net_usage(operand, str);

	if (use_brackets)
		str << ")";
}

}
}
