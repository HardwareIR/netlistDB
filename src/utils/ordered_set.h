#pragma once

#include <vector>
#include <unordered_set>

namespace netlistDB {
namespace utils {

/*
 * Set + vector to keep order of items and allow fast iteration
 *
 * @note if there is less than USE_SET_LOWER_LIMIT items the set is not used
 * 		because linear search in array is faster
 *
 * */
template<typename T, size_t USE_SET_LOWER_LIMIT = 4>
class OrderedSet: public std::vector<T> {
	using vec_t = std::vector<T>;
	std::unordered_set<T> _set;
public:
	using value_type = typename vec_t::value_type;
	OrderedSet() :
			vec_t() {
		vec_t::reserve(4);
	}

	bool push_back(const value_type& __x) {
		if (vec_t::size() < USE_SET_LOWER_LIMIT) {
			for (auto & item : *this) {
				if (item == __x)
					return false;
			}
			vec_t::push_back(__x);
			if (vec_t::size() == USE_SET_LOWER_LIMIT) {
				for (auto & item : *this) {
					_set.insert(item);
				}
			}
			return true;
		} else {
			if (_set.find(__x) == _set.end()) {
				vec_t::push_back(__x);
				_set.insert(__x);
				return true;
			}
			return false;
		}
	}

	void replace(const value_type& __x) {
		throw std::runtime_error("NotImplemented");
	}

	void push_back_many(const vec_t & vec) {
		for (auto & i : vec)
			push_back(i);
	}
};

}
}

