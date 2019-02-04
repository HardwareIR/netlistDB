#pragma once

#include <vector>
#include <unordered_set>

/*
 * Set + vector to keep order of items and allow fast iteration
 *
 * @note if there is less than USE_SET_LOWER_LIMIT items the set is not used
 * 		because linear search in array is faster
 *
 * */
template<typename T>
class OrderedSet: public std::vector<T> {
	using vec_t = std::vector<T>;
	using value_type = typename vec_t::value_type;
	static constexpr size_t USE_SET_LOWER_LIMIT = 4;
	std::unordered_set<T> _set;
public:
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

	}

	void push_back_many(const vec_t & vec) {
		for (auto & i : vec)
			push_back(i);
	}
};
