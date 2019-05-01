#include <netlistDB/serializer/verilog.h>

#include <exception>
#include <netlistDB/operator_defs.h>

using namespace std;

namespace netlistDB {
namespace serializer {

void Verilog2001::serialize_sensitivity_list_item(const iNode & item,
		bool anyIsEventDependent, const HwProcess & proc) {
	auto & str = io.str();
	auto op = dynamic_cast<const FunctionCall*>(&item);
	auto n = dynamic_cast<const Net*>(&item);
	if (op) {
		auto o = &op->fn;
		assert(op->args.size() == 1);
		if (o == &OpRising)
			str << "posedge ";
		else if (o == &OpFalling)
			str << "negedge ";
		else
			throw std::runtime_error("wrong operator in sensitivity list");

		serialize_net_usage(*op->args[0]);
		return;
	} else if (anyIsEventDependent) {
		// async reset etc.
		throw std::runtime_error("[TODO] search usage of net in process and check if negedge or posedge is used");

		//if (n->t.negated) {
		//	str << "negedge ";
		//} else {
		//	str << "posedge ";
		//}
		serialize_net_usage(*n);
		return;
	} else
		serialize_net_usage(*n);
}

void Verilog2001::serialize_operand(const Net & _operand, const FunctionCall & oper,
		bool expr_requires_brackets, bool cancel_brackets) {
	auto & operand = tmp_extractor.checked(_operand);
	Serializer::serialize_operand(operand, oper, expr_requires_brackets, cancel_brackets);
}

void Verilog2001::serialize_generic_binOp(const std::string & op_str,
		const FunctionCall & o) {
	auto & str = io.str();
	serialize_operand(*o.args[0], o, false, false);
	str << " " << op_str << " ";
	serialize_operand(*o.args[1], o, false, false);
}
void Verilog2001::serialize_generic_unOp(const std::string & op_str,
		const FunctionCall & o) {
	auto & str = io.str();
	str << op_str;
	serialize_operand(*o.args[0], o, false, false);
}

void Verilog2001::serialize(const FunctionCall & fncall) {
	auto & ops = fncall.args;
	const FunctionDef * o = &fncall.fn;

	auto un_op = _unaryOps.find(o);
	if (un_op != _unaryOps.end()) {
		assert(ops.size() == 1);
		serialize_generic_unOp(un_op->second, fncall);
		return;
	}

	auto bin_op = _binOps.find(o);
	if (bin_op != _binOps.end()) {
		assert(ops.size() == 2);
		serialize_generic_binOp(bin_op->second, fncall);
		return;
	}

	auto spec_op = _specialOp.find(o);
	if (spec_op != _specialOp.end()) {
		spec_op->second(*this, fncall);
		return;
	}

	if (o == &OpRising or o == &OpFalling) {
		throw std::runtime_error("UnsupportedEventOpErr");
	} else {
		throw std::runtime_error(
				"Do not know how to convert expression with unknown operator to Verilog");
	}
}

}
}
