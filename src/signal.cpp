#include "netlist.h"
#include "operator_defs.h"

using namespace netlistDB;

Signal & apply(FunctionDef & fn, Signal & a, Signal & b) {
	Signal::UsageCacheKey k(fn, { &a, &b });
	auto prev = a.usage_cache.find(k);
	if (prev != a.usage_cache.end())
		return *prev->second;

	auto & res = fn.apply(a, b);
	a.usage_cache[k] = &res;
	b.usage_cache[k] = &res;
	return res;
}

Signal & apply(FunctionDef & fn, Signal & a) {
	Signal::UsageCacheKey k(fn, { &a });
	auto prev = a.usage_cache.find(k);
	if (prev != a.usage_cache.end())
		return *prev->second;

	auto & res = fn.apply(a);
	a.usage_cache[k] = &res;

	return res;
}

Signal::Signal(Netlist & ctx, size_t index, const std::string & name,
		Direction direction) :
		id(name), ctx(ctx), direction(direction), index(index) {
}

Signal & Signal::operator~() {
	return apply(OpNeg, *this);
}

Signal & Signal::operator|(Signal & other) {
	return apply(OpOr, *this, other);
}
Signal & Signal::operator&(Signal & other) {
	return apply(OpAnd, *this, other);
}
Signal & Signal::operator^(Signal & other) {
	return apply(OpXor, *this, other);
}

Signal & Signal::operator<=(Signal & other) {
	return apply(OpLE, *this, other);
}
Signal & Signal::operator<(Signal & other) {
	return apply(OpLt, *this, other);
}
Signal & Signal::operator>=(Signal & other) {
	return apply(OpGE, *this, other);
}
Signal & Signal::operator>(Signal & other) {
	return apply(OpGt, *this, other);
}
Signal & Signal::operator==(Signal & other) {
	return apply(OpEq, *this, other);
}
Signal & Signal::operator!=(Signal & other) {
	return apply(OpNeq, *this, other);
}

Signal & Signal::operator-() {
	return apply(OpUnMinus, *this);
}
Signal & Signal::operator+(Signal & other) {
	return apply(OpAdd, *this, other);
}
Signal & Signal::operator-(Signal & other) {
	return apply(OpSub, *this, other);
}
Signal & Signal::operator*(Signal & other) {
	return apply(OpMul, *this, other);
}
Signal & Signal::operator/(Signal & other) {
	return apply(OpDiv, *this, other);
}

Signal & Signal::concat(Signal & other) {
	return apply(OpConcat, *this, other);
}
Signal & Signal::rising() {
	return apply(OpRising, *this);
}
Signal & Signal::falling() {
	return apply(OpFalling, *this);
}

// used as assignment
Assignment & Signal::operator()(Signal & other) {
	return *(new Assignment(*this, other));
}
