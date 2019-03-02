#include <netlistDB/serializer/verilog.h>

#include <exception>
#include <netlistDB/operator_defs.h>

namespace netlistDB {
namespace serializer {

bool _operandIsAnotherOperand(const Net & operand) {
	return (operand.val == nullptr and operand.id.hidden
			and operand.drivers.size() == 0
			and dynamic_cast<const FunctionCall*>(operand.drivers[0]));
}

Net & extract_as_tmp_var(Net & n) {
	throw std::runtime_error(
			std::string("Not implemented ") + __PRETTY_FUNCTION__);
	//Net & tmpVar = createTmpVarFn("tmp_concat_", n.t);
	//tmpVar.def_val = n;
	//return tmpVar;
}

void Verilog2001::serialize_operand(const Net & _operand, const FunctionCall & oper,
		bool expr_requires_braces, std::ostream & str) {
	// [TODO] if operand is concatenation and parent operator
	//        is not concatenation operand should be extracted
	//        as tmp variable
	//        * maybe flatten the concatenations
	auto operand = &_operand;
	if (&oper.fn != &OpConcat and _operandIsAnotherOperand(_operand)
			and _operand.drivers.size() == 1) {
		auto d = dynamic_cast<const FunctionCall*>(_operand.drivers[0]);
		if (d and &d->fn == &OpConcat) {
			throw std::runtime_error(
					std::string("Not implemented ") + __PRETTY_FUNCTION__);
			//operand = &extract_as_tmp_var(_operand);
		}
	}
	Serializer::serialize_operand(*operand, oper, true, str);
}

void Verilog2001::serialize_generic_binOp(const std::string & op_str,
		const FunctionCall & o, std::ostream & str) {
	serialize_operand(*o.args[0], o, false, str);
	str << " " << op_str << " ";
	serialize_operand(*o.args[1], o, false, str);
}
void Verilog2001::serialize_generic_unOp(const std::string & op_str,
		const FunctionCall & o, std::ostream & str) {
	str << op_str;
	serialize_operand(*o.args[0], o, false, str);
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
