#pragma once

#include <vector>
#include <unordered_set>
#include "../parallel_utils/erase_if.h"

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

	/*
	 * Discard the item from the collection
	 * @return true if the item was removed
	 **/
	bool discard(T item) {
		size_t prev_size = vec_t::size();
		bool use_set = prev_size >= USE_SET_LOWER_LIMIT;
		if (use_set) {
			auto i = _set.find(item);
			if (i != _set.end()) {
				_set.erase(i);
			}
		}
		parallel_utils::erase_if_seq<T>(*this, [item](T i) {
			return i == item;
		});
		bool was_erased = prev_size != vec_t::size();
		if (use_set and was_erased and prev_size == USE_SET_LOWER_LIMIT) {
			_set.clear();
		}
		return was_erased;
	}

	void replace(const value_type& __x) {
		throw std::runtime_error("NotImplemented");
	}

	template<typename iterable>
	void extend(const iterable & other) {
		for (T i: other) {
			push_back(i);
		}
	}

	void push_back_many(const vec_t & vec) {
		for (auto & i : vec)
			push_back(i);
	}

	typename vec_t::iterator find(T item) {
		return std::find(vec_t::begin(), vec_t::end(), item);
	}
};

}
}

