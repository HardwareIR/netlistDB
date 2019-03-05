#include <netlistDB/serializer/verilog.h>
#include <netlistDB/hw_type/hw_int.h>
#include <netlistDB/bit_utils.h>

using namespace std;
using namespace netlistDB::hw_type;
using namespace netlistDB::bit_utils;

namespace netlistDB {
namespace serializer {

Verilog2001::Verilog2001(
		std::map<const std::string, const void*> reserved_names) :
		name_scope(true), indent_cnt(0) {
	name_scope.set_level(1);
	auto & scope = *name_scope[0];
	for (auto & kw : keywords) {
		scope[kw] = new KeyWord;
	}
}

void Verilog2001::serialize_net_def(const Net & n, std::ostream & str) {
	indent(str);
	auto v_t = verilogTypeOfSig(n);
	if (v_t == VERILOG_NET_TYPE::VERILOG_REG) {
		str << "reg ";
	} else {
		str << "wire ";
	}
	if (serialize_type_usage(n.t, str))
		str << " ";

	str << name_scope.checkedName(n.id.name, &n) << ";";
}

bool Verilog2001::serialize_type_usage(const hw_type::iHwType & t,
		std::ostream & str) {
	auto int_t = dynamic_cast<const HwInt*>(&t);
	if (int_t) {
		if (int_t->bit_length() == 1 and not int_t->has_to_be_vector) {
			// only single bit, does not require any type specification
			return false;
		} else {
			if (int_t->is_signed)
				str << "signed ";
			str << "[" << int_t->bit_length() << "-1:0]";
			return true;
		}
	} else {
		throw runtime_error(
				std::string(__FILE__) + ":" + std::to_string(__LINE__) + " not implemented for this type");
	}

}

void Verilog2001::serialize_value(
		const typename hw_type::HwInt::value_type & val, std::ostream & str) {
	auto & t = val.t;
	str << t.bit_length();
	str << "'";

	auto str_flags = str.flags();

	if ((val.mask != 0 and val.mask != t.all_mask) or t.bit_length() == 1) {
		// base = 2
		str << "b";
		for (int b = int(t.bit_length()) - 1; b >= 0; b--) {
			auto m = select_bits(val.mask, b, 1);
			auto v = select_bits(val.value, b, 1);
			if (m) {
				if (v) {
					str << "1";
				} else {
					str << "0";
				}
			} else {
				str << "X";
			}
		}
	} else {
		// base = 16
		str << "h";
		constexpr size_t bits_per_char = 4;
		size_t rounded_size = t.bit_length() / bits_per_char;
		if (rounded_size * bits_per_char < t.bit_length()) {
			rounded_size++;
		}
		for (int B = rounded_size - 1; B >= 0; B--) {
			auto m = select_bits(val.mask, B * bits_per_char, bits_per_char);
			auto v = select_bits(val.value, B * bits_per_char, bits_per_char);
			if (m) {
				str << hex << int(v);
			} else {
				str << "X";
			}
		}
	}

	str.flags(str_flags);
}

void Verilog2001::serialize_net_usage(const Net & n, std::ostream & str) {
	if (n.id.hidden) {
		if (n.val) {
			assert(
					n.drivers.size() == 0
							&& "constant net should not have any other driver");
			serialize_value(*n.val, str);
		} else {
			assert(n.drivers.size() == 1);
			auto op = dynamic_cast<const FunctionCall*>(n.drivers[0]);
			assert(n.drivers.size() == 1);
			assert(op);
			serialize(*op, str);
		}
	} else {
		str << name_scope.checkedName(n.id.name, &n);
	}
}

Verilog2001::~Verilog2001() {
	auto & scope = *name_scope[0];
	for (auto & kw : keywords) {
		delete reinterpret_cast<const KeyWord*>(scope[kw]);
	}
}

void Verilog2001::serialize_block(const vector<Statement*> & stms,
		std::ostream & str) {
	if (stms.size() == 1) {
		str << endl;
		indent_cnt++;
		serialize_stm(*stms[0], str);
		str << endl;
		indent_cnt--;
	} else {
		str << " begin" << std::endl;

		indent_cnt++;
		for (auto s : stms) {
			serialize_stm(*s, str);
			str << endl;
		}
		indent_cnt--;

		indent(str) << "end";
	}
}

std::ostream & Verilog2001::indent(std::ostream & str) {
	for (size_t i = 0; i < indent_cnt; i++) {
		str << INDENT;
	}
	return str;
}

enum Verilog2001::VERILOG_NET_TYPE Verilog2001::verilogTypeOfSig(
		const Net & n) const {
	size_t driver_cnt = n.drivers.size();
	if (n.direction == Direction::DIR_IN) {
		// is input port
		return VERILOG_NET_TYPE::VERILOG_WIRE;
	} else if (driver_cnt == 1) {
		auto d = dynamic_cast<Assignment*>(n.drivers[0]);
		if (d and not d->parent and not d->dst_index.size()
				and not d->sens.now_is_event_dependent)
			// primitive assignment
			return VERILOG_NET_TYPE::VERILOG_WIRE;
	}

	return VERILOG_NET_TYPE::VERILOG_REG;
}

const std::map<const FunctionDef*, int> & Verilog2001::get_operator_precedence() {
	return opPrecedence;
}

}
}
