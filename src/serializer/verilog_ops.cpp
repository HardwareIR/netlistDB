#include <netlistDB/serializer/verilog.h>

#include <exception>
#include <netlistDB/operator_defs.h>

using namespace std;

namespace netlistDB {
namespace serializer {

void Verilog2001::serialize_sensitivity_list_item(const iNode & item,
		bool anyIsEventDependent, const HwProcess & proc, ostream & str) {
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

		serialize_net_usage(*op->args[0], str);
		return;
	} else if (anyIsEventDependent) {
		// async reset etc.
		throw std::runtime_error("[TODO] search usage of net in process and check if negedge or posedge is used");

		//if (n->t.negated) {
		//	str << "negedge ";
		//} else {
		//	str << "posedge ";
		//}
		serialize_net_usage(*n, str);
		return;
	} else
		serialize_net_usage(*n, str);
}

void Verilog2001::serialize_operand(const Net & _operand, const FunctionCall & oper,
		bool expr_requires_braces, bool cancel_brances, std::ostream & str) {
	auto & operand = tmp_extractor.checked(_operand);
	Serializer::serialize_operand(operand, oper, expr_requires_braces, cancel_brances, str);
}

void Verilog2001::serialize_generic_binOp(const std::string & op_str,
		const FunctionCall & o, std::ostream & str) {
	serialize_operand(*o.args[0], o, false, false, str);
	str << " " << op_str << " ";
	serialize_operand(*o.args[1], o, false, false, str);
}
void Verilog2001::serialize_generic_unOp(const std::string & op_str,
		const FunctionCall & o, std::ostream & str) {
	str << op_str;
	serialize_operand(*o.args[0], o, false, false, str);
}

void Verilog2001::serialize(const FunctionCall & fncall, std::ostream & str) {
	auto & ops = fncall.args;
	const FunctionDef * o = &fncall.fn;

	auto un_op = _unaryOps.find(o);
	if (un_op != _unaryOps.end()) {
		assert(ops.size() == 1);
		serialize_generic_unOp(un_op->second, fncall, str);
		return;
	}

	auto bin_op = _binOps.find(o);
	if (bin_op != _binOps.end()) {
		assert(ops.size() == 2);
		serialize_generic_binOp(bin_op->second, fncall, str);
		return;
	}

	auto spec_op = _specialOp.find(o);
	if (spec_op != _specialOp.end()) {
		spec_op->second(*this, fncall, str);
		return;
	}

	//if o == AllOps.CALL:
	//    return "%s(%s)" % (cls.FunctionContainer(ops[0]),
	//                       ", ".join(map(lambda op: cls._operand(op, op, ctx), ops[1:])))
	//elif o == AllOps.TERNARY:
	//    zero, one = BIT.fromPy(0), BIT.fromPy(1)
	//    if ops[1] == one and ops[2] == zero:
	//        # ignore redundant x ? 1 : 0
	//        return cls.condAsHdl([ops[0]], True, ctx)
	//    else:
	//        return "%s ? %s : %s" % (cls.condAsHdl([ops[0]], True, ctx),
	//                                 cls._operand(ops[1], op, ctx),
	//                                 cls._operand(ops[2], op, ctx))
	if (o == &OpRising or o == &OpFalling) {
		throw std::runtime_error("UnsupportedEventOpErr");
		//} else if (o == OpBitsAsUnsigned) {
		//    op0, = ops
		//    op_str = cls._operand(op0, op, ctx)
		//    if bool(op0._dtype.signed):
		//        return "$unsigned(%s)" % op_str
		//    else:
		//        return op_str
		//} else if (o == &OpIntToBits) {
		//    op0, = ops
		//    width = op.result._dtype.bit_length()
		//    op_str = cls._operand(op0, op, ctx)
		//    if op_str.startswith("(") and not isinstance(op, Value):
		//        return "%d'%s" % (width, op_str)
		//    else:
		//        return "%d'(%s)" % (width, op_str)
	} else {
		throw std::runtime_error(
				"Do not know how to convert expression with unknown operator to Verilog");
	}
}

}
}
