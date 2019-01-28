#pragma once
#include <vector>
#include <boost/functional/hash.hpp>

namespace netlistDB {

template<typename FunctionDef, typename Signal>
class _UsageCacheKey {
	FunctionDef * fn;
	std::vector<Signal*> args;

public:
	_UsageCacheKey(FunctionDef & fn, std::initializer_list<Signal*> l) :
			fn(&fn), args(l) {
	}

	size_t hash() const noexcept {
		size_t h = 0;
		boost::hash_combine(h, std::hash<FunctionDef*>{}(fn));
		for (auto i : args) {
			boost::hash_combine(h, std::hash<Signal*>{}(i));
		}
		return h;
	}

	bool operator==(const _UsageCacheKey & other) const noexcept {
		if (fn != other.fn)
			return false;
		if (args.size() != other.args.size())
			return false;

		auto it = other.args.begin();
		for (auto a : args) {
			if (a != *it)
				return false;
			++it;
		}
		return true;
	}
};

}

namespace std {

template<typename Signal, typename FunctionDef>
struct hash<netlistDB::_UsageCacheKey<Signal, FunctionDef>> {
	typedef netlistDB::_UsageCacheKey<Signal, FunctionDef> argument_type;
	typedef size_t result_type;
	result_type operator()(argument_type const& v) const noexcept {
		return v.hash();
	}
};

}
