#include <netlistDB/netlist.h>
#include <netlistDB/utils/erase_if.h>
#include <netlistDB/operator_defs.h>
#include <netlistDB/statement_assignment.h>

namespace netlistDB {

Net & apply_call(FunctionDef & fn, Net & a, Net & b) {
	assert(&(a.ctx) == &(b.ctx));
	Net::UsageCacheKey k(fn, { &a, &b });
	auto prev = a.usage_cache.find(k);
	if (prev != a.usage_cache.end())
		return *prev->second;

	auto & res = a.ctx.sig(a.t);
	new FunctionCall(fn, a, b, res);

	a.usage_cache[k] = &res;
	b.usage_cache[k] = &res;
	return res;
}

Net & apply_call(FunctionDef & fn, Net & a) {
	Net::UsageCacheKey k(fn, { &a });
	auto prev = a.usage_cache.find(k);
	if (prev != a.usage_cache.end())
		return *prev->second;

	auto & res = a.ctx.sig(a.t);
	new FunctionCall(fn, a, res);

	a.usage_cache[k] = &res;
	return res;
}

Net::Net(Netlist & ctx, hw_type::iHwType & t, const std::string & name,
		Direction direction) :
		id(name), ctx(ctx), net_index(0), t(t), val(nullptr), nop_val(nullptr), def_val(
				nullptr), direction(direction) {
	ctx.register_node(*this);
}

bool Net::is_const() {
	return val != nullptr;
}

Net & Net::operator~() {
	return apply_call(OpNeg, *this);
}

Net & Net::operator|(Net & other) {
	return apply_call(OpOr, *this, other);
}
Net & Net::operator&(Net & other) {
	return apply_call(OpAnd, *this, other);
}
Net & Net::operator^(Net & other) {
	return apply_call(OpXor, *this, other);
}

Net & Net::operator<=(Net & other) {
	return apply_call(OpLE, *this, other);
}
Net & Net::operator<(Net & other) {
	return apply_call(OpLt, *this, other);
}
Net & Net::operator>=(Net & other) {
	return apply_call(OpGE, *this, other);
}
Net & Net::operator>(Net & other) {
	return apply_call(OpGt, *this, other);
}
Net & Net::operator==(Net & other) {
	return apply_call(OpEq, *this, other);
}
Net & Net::operator!=(Net & other) {
	return apply_call(OpNeq, *this, other);
}

Net & Net::operator-() {
	return apply_call(OpUnMinus, *this);
}
Net & Net::operator+(Net & other) {
	return apply_call(OpAdd, *this, other);
}
Net & Net::operator-(Net & other) {
	return apply_call(OpSub, *this, other);
}
Net & Net::operator*(Net & other) {
	return apply_call(OpMul, *this, other);
}
Net & Net::operator/(Net & other) {
	return apply_call(OpDiv, *this, other);
}

Net & Net::concat(Net & other) {
	return apply_call(OpConcat, *this, other);
}

Net & Net::operator[](Net & index) {
	return apply_call(OpSlice, *this, index);
}

Net & Net::rising() {
	return apply_call(OpRising, *this);
}
Net & Net::falling() {
	return apply_call(OpFalling, *this);
}

// used as assignment
Statement & Net::operator()(Net & other) {
	return *(new Assignment(*this, other));
}

void Net::forward(const predicate_t & fn) {
	for (auto e: endpoints) {
		if (fn(*e))
			return;
	}
}

void Net::backward(const predicate_t & fn) {
	for (auto d: drivers) {
		if (fn(*d))
			return;
	}
}

void Net::forward_disconnect(std::function<bool(iNode*)> pred) {
	parallel_utils::erase_if_seq<OperationNode*>(endpoints, pred);
}

}
