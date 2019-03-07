#pragma once
#include <stdint.h>

namespace netlistDB {
namespace hw_type {


// wrapper for boost::multiprecision::cpp_int arbitrary integer
// cpp_int is hidden behind opaque pointer so we can hide boost::multiprecision namespace which is slow to compile
// @note it is safe to reinterpret this class back to boost::multiprecision::cpp_int* if required
// @note cpp_int is stored in big-endian format however operators behave like it is a little-endian
//       (shift, concatenation, bit index)
class aint_t {
public:
	struct cpp_int_placehoder;
private:
	cpp_int_placehoder * _M;
public:
	aint_t();
	aint_t(const aint_t & v);
	aint_t(uint64_t v);
	aint_t(int64_t v);
	aint_t(int v);
	aint_t(unsigned v);

	aint_t operator+(const aint_t & other) const;
	aint_t operator+(uint64_t other) const;
	aint_t operator+(int64_t other) const;
	aint_t operator+(int other) const;
	aint_t operator+(unsigned other) const;

	aint_t operator-(const aint_t & other) const;
	aint_t operator-(uint64_t other) const;
	aint_t operator-(int64_t other) const;
	aint_t operator-(int other) const;
	aint_t operator-(unsigned other) const;

	aint_t operator*(const aint_t & other) const;
	aint_t operator*(uint64_t other) const;
	aint_t operator*(int64_t other) const;
	aint_t operator*(int other) const;
	aint_t operator*(unsigned other) const;

	aint_t operator/(const aint_t & other) const;
	aint_t operator/(uint64_t other) const;
	aint_t operator/(int64_t other) const;
	aint_t operator/(int other) const;
	aint_t operator/(unsigned other) const;

	aint_t operator%(const aint_t & other) const;
	aint_t operator%(uint64_t other) const;
	aint_t operator%(int64_t other) const;
	aint_t operator%(int other) const;
	aint_t operator%(unsigned other) const;

	aint_t operator^(const aint_t & other) const;
	aint_t operator^(uint64_t other) const;
	aint_t operator^(int64_t other) const;
	aint_t operator^(int other) const;
	aint_t operator^(unsigned other) const;

	aint_t operator&(const aint_t & other) const;
	aint_t operator&(uint64_t other) const;
	aint_t operator&(int64_t other) const;
	aint_t operator&(int other) const;
	aint_t operator&(unsigned other) const;

	aint_t operator|(const aint_t & other) const;
	aint_t operator|(uint64_t other) const;
	aint_t operator|(int64_t other) const;
	aint_t operator|(int other) const;
	aint_t operator|(unsigned other) const;

	bool operator<(const aint_t & other) const;
	bool operator<(uint64_t other) const;
	bool operator<(int64_t other) const;
	bool operator<(int other) const;
	bool operator<(unsigned other) const;

	bool operator>(const aint_t & other) const;
	bool operator>(uint64_t other) const;
	bool operator>(int64_t other) const;
	bool operator>(int other) const;
	bool operator>(unsigned other) const;

	bool operator<=(const aint_t & other) const;
	bool operator<=(uint64_t other) const;
	bool operator<=(int64_t other) const;
	bool operator<=(int other) const;
	bool operator<=(unsigned other) const;

	bool operator>=(const aint_t & other) const;
	bool operator>=(uint64_t other) const;
	bool operator>=(int64_t other) const;
	bool operator>=(int other) const;
	bool operator>=(unsigned other) const;

	//aint_t operator<<(const aint_t & other) const;
	aint_t operator<<(uint64_t other) const;
	//aint_t operator<<(int64_t other) const;
	//aint_t operator<<(int other) const;
	//aint_t operator<<(unsigned other) const;

	//aint_t operator>>(const aint_t & other) const;
	aint_t operator>>(uint64_t other) const;
	//aint_t operator>>(int64_t other) const;
	//aint_t operator>>(int other) const;
	//aint_t operator>>(unsigned other) const;

	bool operator==(const aint_t & other) const;
	bool operator==(uint64_t other) const;
	bool operator==(int64_t other) const;
	bool operator==(int other) const;
	bool operator==(unsigned other) const;

	bool operator!=(const aint_t & other) const;
	bool operator!=(uint64_t other) const;
	bool operator!=(int64_t other) const;
	bool operator!=(int other) const;
	bool operator!=(unsigned other) const;


	bool operator&&(const aint_t & other) const;
	bool operator&&(uint64_t other) const;
	bool operator&&(int64_t other) const;
	bool operator&&(int other) const;
	bool operator&&(unsigned other) const;

	bool operator||(const aint_t & other) const;
	bool operator||(uint64_t other) const;
	bool operator||(int64_t other) const;
	bool operator||(int other) const;
	bool operator||(unsigned other) const;


	void operator+=(const aint_t & other) const;
	void operator+=(uint64_t other) const;
	void operator+=(int64_t other) const;
	void operator+=(int other) const;
	void operator+=(unsigned other) const;

	void operator-=(const aint_t & other) const;
	void operator-=(uint64_t other) const;
	void operator-=(int64_t other) const;
	void operator-=(int other) const;
	void operator-=(unsigned other) const;

	void operator/=(const aint_t & other) const;
	void operator/=(uint64_t other) const;
	void operator/=(int64_t other) const;
	void operator/=(int other) const;
	void operator/=(unsigned other) const;

	void operator%=(const aint_t & other) const;
	void operator%=(uint64_t other) const;
	void operator%=(int64_t other) const;
	void operator%=(int other) const;
	void operator%=(unsigned other) const;

	void operator^=(const aint_t & other) const;
	void operator^=(uint64_t other) const;
	void operator^=(int64_t other) const;
	void operator^=(int other) const;
	void operator^=(unsigned other) const;

	void operator&=(const aint_t & other) const;
	void operator&=(uint64_t other) const;
	void operator&=(int64_t other) const;
	void operator&=(int other) const;
	void operator&=(unsigned other) const;

	void operator|=(const aint_t & other) const;
	void operator|=(uint64_t other) const;
	void operator|=(int64_t other) const;
	void operator|=(int other) const;
	void operator|=(unsigned other) const;

	void operator*=(const aint_t & other) const;
	void operator*=(uint64_t other) const;
	void operator*=(int64_t other) const;
	void operator*=(int other) const;
	void operator*=(unsigned other) const;

	//void operator<<=(const aint_t & other) const;
	void operator<<=(uint64_t other) const;
	//void operator<<=(int64_t other) const;
	//void operator<<=(int other) const;
	//void operator<<=(unsigned other) const;

	//void operator>>=(const aint_t & other) const;
	void operator>>=(uint64_t other) const;
	//void operator>>=(int64_t other) const;
	//void operator>>=(int other) const;
	//void operator>>=(unsigned other) const;

	operator bool() const;

	~aint_t();
};

}
}
