#pragma once

#include <vector>
#include <assert.h>

namespace netlistDB {
namespace utils {

/* Iterator of chained vectors
 **/
template<class T>
class ChaindedIter: public std::iterator<std::forward_iterator_tag, T> {
public:
	using it_t = typename std::vector<T>::iterator;

	static constexpr it_t to_iter(T * item) {
		return *reinterpret_cast<it_t*>(&item);
	}

	struct _Item {
		enum Item_t {
			VEC, PTR, VAL,
		};

		union {
			std::vector<T>* vec;
			T * ptr;
			T val;
		} item;
		Item_t t;

		_Item(std::vector<T>* vec) :
				t(VEC) {
			item.vec = vec;
		}

		_Item(T* ptr) :
				t(PTR) {
			item.ptr = ptr;
		}

		_Item(T val) :
				t(VAL) {
			item.val = val;
		}

		std::pair<it_t, it_t> get_begin_end() {
			if (t == VEC) {
				return {item.vec->begin(), item.vec->end()};
			} else if (t == PTR) {
				return {to_iter(item.ptr), to_iter(item.ptr) + 1};
			} else {
				assert(t == VAL);
				return {to_iter(&item.val), to_iter(&item.val) + 1};
			}
		}
	};

protected:
	std::vector<_Item> & vectors;

	it_t actual;
	it_t actual_end;
	size_t actual_vec_i;

public:
	ChaindedIter(std::vector<_Item> & vectors) :
			vectors(vectors), actual(nullptr), actual_end(nullptr), actual_vec_i(0) {
		if (vectors.size()) {
			auto & first = vectors[0];
			std::tie(actual, actual_end) = first.get_begin_end();
			skip_item_if_on_end();
		}
	}

	ChaindedIter(std::vector<_Item> & vectors, it_t actual_pos, it_t actual_end, size_t actual_vec_i) :
			vectors(vectors), actual(actual_pos), actual_end(actual_end), actual_vec_i(actual_vec_i) {
	}

	T& operator*() {
		return *actual;
	}

	T* operator->() {
		return &(*actual);
	}

	ChaindedIter& operator++() {
		actual++;
		// there can be the empty vectors which we need to skip
		skip_item_if_on_end();
		return *this;
	}

	friend bool operator==(ChaindedIter a, ChaindedIter b) {
		return a.actual == b.actual and a.actual_vec_i == b.actual_vec_i;
	}

	friend bool operator!=(ChaindedIter a, ChaindedIter b) {
		return a.actual != b.actual or a.actual_vec_i != b.actual_vec_i;
	}

private:
	constexpr void skip_item_if_on_end() {
		while (actual == actual_end and actual_vec_i + 1 < vectors.size()) {
			actual_vec_i++;
			auto & av = vectors[actual_vec_i];
			std::tie(actual, actual_end) = av.get_begin_end();
		}
	}
};

template<class T>
class ChainedSequence {
	using _Item = typename ChaindedIter<T>::_Item;
	std::vector<_Item> vectors;

public:
	ChainedSequence() {
	}

	ChainedSequence(std::initializer_list<T> l) {
		for (auto & v : l) {
			push_back(&v);
		}
	}

	ChainedSequence(std::initializer_list<std::vector<T>*> l) {
		for (auto v : l) {
			push_back(v);
		}
	}

	constexpr void push_back(std::vector<T>* item) {
		vectors.push_back(_Item(item));
	}

	constexpr void push_back(T* item) {
		vectors.push_back(_Item(item));
	}

	constexpr void push_back(T item) {
		vectors.push_back(_Item(item));
	}

	ChainedSequence joined(const ChainedSequence & other) const {
		ChainedSequence it;
		it.vectors.reserve(vectors.size() + other.vectors.size());
		it.vectors.insert(it.vectors.end(), vectors.begin(), vectors.end());
		it.vectors.insert(it.vectors.end(), other.vectors.begin(),
				other.vectors.end());
		return it;
	}

	ChaindedIter<T> begin() {
		return ChaindedIter<T>(vectors);
	}

	ChaindedIter<T> end() {
		using it_t = typename std::vector<T>::iterator;
		it_t actual, actual_end;
		if (vectors.size()) {
			auto & last = vectors.back();
			std::tie(actual, actual_end) = last.get_begin_end();
			actual = actual_end;
		}
		size_t i = vectors.size();
		if (i > 0)
			i--;
		return ChaindedIter<T>(vectors, actual, actual_end, i);
	}
};

}
}
