#include <netlistDB/hw_type/aint.h>
#include <boost/multiprecision/cpp_int.hpp>

namespace netlistDB {
namespace hw_type {

boost::multiprecision::cpp_int & as_cpp_int(aint_t::cpp_int_placehoder * v) {
	return *reinterpret_cast<boost::multiprecision::cpp_int*>(v);
}

aint_t::aint_t() {
	_M =
			reinterpret_cast<cpp_int_placehoder *>(new boost::multiprecision::cpp_int);
}
aint_t::aint_t(const aint_t & v) {
	_M =
			reinterpret_cast<cpp_int_placehoder *>(new boost::multiprecision::cpp_int(
					as_cpp_int(v._M)));
}
aint_t::aint_t(uint64_t v) {
	_M =
			reinterpret_cast<cpp_int_placehoder *>(new boost::multiprecision::cpp_int(
					v));
}
aint_t::aint_t(int64_t v) {
	_M =
			reinterpret_cast<cpp_int_placehoder *>(new boost::multiprecision::cpp_int(
					v));
}
aint_t::aint_t(int v) {
	_M =
			reinterpret_cast<cpp_int_placehoder *>(new boost::multiprecision::cpp_int(
					v));
}
aint_t::aint_t(unsigned v) {
	_M =
			reinterpret_cast<cpp_int_placehoder *>(new boost::multiprecision::cpp_int(
					v));
}

#define def_bin_op(ret_t, op)                             \
		ret_t aint_t::operator op(const aint_t & other) const {      \
	aint_t res;                                               \
	as_cpp_int(res._M) = as_cpp_int(_M) op as_cpp_int(other._M);   \
	return res;                                    \
}                                                  \
ret_t aint_t::operator op(uint64_t other) const { \
	aint_t res;                                    \
	as_cpp_int(res._M) = as_cpp_int(_M) op other;  \
	return res;                                    \
}                                                  \
ret_t aint_t::operator op(int64_t other) const {  \
	aint_t res;                                    \
	as_cpp_int(res._M) = as_cpp_int(_M) op other;  \
	return res;                                    \
}                                                  \
ret_t aint_t::operator op(int other) const {      \
	aint_t res;                                    \
	as_cpp_int(res._M) = as_cpp_int(_M) op other;  \
	return res;                                    \
}                                                  \
ret_t aint_t::operator op(unsigned other) const { \
	aint_t res;                                    \
	as_cpp_int(res._M) = as_cpp_int(_M) op other;  \
	return res;                                    \
}

def_bin_op(aint_t, -)
def_bin_op(aint_t, +)
def_bin_op(aint_t, *)
def_bin_op(aint_t, /)
def_bin_op(aint_t, %)
def_bin_op(aint_t, ^)
def_bin_op(aint_t, &)
def_bin_op(aint_t, |)
def_bin_op(bool, <)
def_bin_op(bool, >)
def_bin_op(bool, <=)
def_bin_op(bool, >=)
aint_t aint_t::operator <<(size_t other) const {
	aint_t res;
	as_cpp_int(res._M) = as_cpp_int(_M) << other;
	return res;
}
aint_t aint_t::operator >>(size_t other) const {
	aint_t res;
	as_cpp_int(res._M) = as_cpp_int(_M) >> other;
	return res;
}
def_bin_op(bool, ==)
def_bin_op(bool, !=)
def_bin_op(bool, &&)
def_bin_op(bool, ||)

#define def_inplace_op(op)                             \
void aint_t::operator op(const aint_t & other) { \
	as_cpp_int(_M) op as_cpp_int(other._M);            \
}                                                      \
void aint_t::operator op(uint64_t other) {       \
	as_cpp_int(_M) op other;                           \
}                                                      \
void aint_t::operator op(int64_t other) {        \
	as_cpp_int(_M) op other;                           \
}                                                      \
void aint_t::operator op(int other) {            \
	as_cpp_int(_M) op other;                           \
}                                                      \
void aint_t::operator op(unsigned other) {       \
	as_cpp_int(_M) op other;                           \
}

def_inplace_op(+=)
def_inplace_op(-=)
def_inplace_op(/=)
def_inplace_op(%=)
def_inplace_op(^=)
def_inplace_op(&=)
def_inplace_op(|=)
def_inplace_op(*=)

void aint_t::operator <<=(uint64_t other) {
	as_cpp_int(_M) <<= other;
}
void aint_t::operator >>=(uint64_t other) {
	as_cpp_int(_M) >>= other;
}

aint_t aint_t::operator~() {
	aint_t res;
	as_cpp_int(res._M) = ~as_cpp_int(_M);
	return res;
}
aint_t aint_t::operator-() {
	aint_t res;
	as_cpp_int(res._M) = -as_cpp_int(_M);
	return res;
}
aint_t aint_t::operator!() {
	aint_t res;
	as_cpp_int(res._M) = !as_cpp_int(_M);
	return res;
}

aint_t aint_t::operator++() {
	as_cpp_int(_M)++;
	return aint_t(*this);
}

aint_t aint_t::operator--() {
	as_cpp_int(_M)--;
	return aint_t(*this);
}

aint_t::operator int64_t() const {
	return int64_t(as_cpp_int(_M));
}
aint_t::operator uint64_t() const {
	return uint64_t(as_cpp_int(_M));
}
aint_t::operator unsigned() const {
	return unsigned(as_cpp_int(_M));
}
aint_t::operator int() const {
	return int(as_cpp_int(_M));
}
aint_t::operator bool() const {
	return bool(as_cpp_int(_M));
}

std::ostream & operator<<(std::ostream & stream, const aint_t & t) {
	return stream << as_cpp_int(t._M);
}

aint_t::~aint_t() {
	delete &as_cpp_int(_M);
}

}
}
