#include "netlist.h"
#include "operator_defs.h"

namespace netlistDB {

Net & apply(FunctionDef & fn, Net & a, Net & b) {
	assert(&(a.ctx) == &(b.ctx));
	Net::UsageCacheKey k(fn, { &a, &b });
	auto prev = a.usage_cache.find(k);
	if (prev != a.usage_cache.end())
		return *prev->second;

	auto & res = a.ctx.sig();
	new FunctionCall(fn, a, b, res);

	a.usage_cache[k] = &res;
	b.usage_cache[k] = &res;
	return res;
}

Net & apply(FunctionDef & fn, Net & a) {
	Net::UsageCacheKey k(fn, { &a });
	auto prev = a.usage_cache.find(k);
	if (prev != a.usage_cache.end())
		return *prev->second;

	auto & res = a.ctx.sig();
	new FunctionCall(fn, a, res);

	a.usage_cache[k] = &res;
	return res;
}

Net::Net(Netlist & ctx, const std::string & name, Direction direction) :
		id(name), ctx(ctx), direction(direction) {
	ctx.register_node(*this);
}

Net & Net::operator~() {
	return apply(OpNeg, *this);
}

Net & Net::operator|(Net & other) {
	return apply(OpOr, *this, other);
}
Net & Net::operator&(Net & other) {
	return apply(OpAnd, *this, other);
}
Net & Net::operator^(Net & other) {
	return apply(OpXor, *this, other);
}

Net & Net::operator<=(Net & other) {
	return apply(OpLE, *this, other);
}
Net & Net::operator<(Net & other) {
	return apply(OpLt, *this, other);
}
Net & Net::operator>=(Net & other) {
	return apply(OpGE, *this, other);
}
Net & Net::operator>(Net & other) {
	return apply(OpGt, *this, other);
}
Net & Net::operator==(Net & other) {
	return apply(OpEq, *this, other);
}
Net & Net::operator!=(Net & other) {
	return apply(OpNeq, *this, other);
}

Net & Net::operator-() {
	return apply(OpUnMinus, *this);
}
Net & Net::operator+(Net & other) {
	return apply(OpAdd, *this, other);
}
Net & Net::operator-(Net & other) {
	return apply(OpSub, *this, other);
}
Net & Net::operator*(Net & other) {
	return apply(OpMul, *this, other);
}
Net & Net::operator/(Net & other) {
	return apply(OpDiv, *this, other);
}

Net & Net::concat(Net & other) {
	return apply(OpConcat, *this, other);
}
Net & Net::rising() {
	return apply(OpRising, *this);
}
Net & Net::falling() {
	return apply(OpFalling, *this);
}

// used as assignment
Assignment & Net::operator()(Net & other) {
	return *(new Assignment(*this, other));
}

iNode::iterator Net::forward() {
	iNode::iterator it;
	for (auto i : endpoints) {
		it.push_back(i);
	}
	return it;
}

iNode::iterator Net::backward() {
	iNode::iterator it;
	for (auto o : drivers)
		it.push_back(o);
	return it;
}

}
