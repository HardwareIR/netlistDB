#pragma once
#include <vector>

namespace netlistDB {
namespace utils {

/* Iterator of chained vectors
 **/
template<class T>
class ChaindedIter: public std::iterator<std::forward_iterator_tag, T> {
	std::vector<std::vector<T>*> vectors;
	typename std::vector<T>::iterator actual;
	typename std::vector<T>::iterator actual_end;
	typename std::vector<T>::iterator total_end;
	size_t actual_vec_i;

public:

	ChaindedIter(std::vector<std::vector<T>*> vectors, size_t start,
			size_t size) :
			vectors(vectors) {
		_init(start, size);
	}

	ChaindedIter(std::initializer_list<std::vector<T>*> l, size_t start,
			size_t size) {
		vectors.reserve(l.size());
		for (auto v : l) {
			vectors.push_back(v);
		}
		_init(start, size);
	}

	void _init(size_t start, size_t size) {
		// resolve actual and actual_end
		actual_vec_i = 0;
		size_t offset = 0;
		while (true) {
			auto & av = *vectors[actual_vec_i];
			size_t offset_in_actual = start - offset;
			if (offset_in_actual < av.size()) {
				actual = av.begin() + offset_in_actual;
				actual_end = av.end();
				break;
			}
			offset += av.size();
			actual_vec_i++;
			if (actual_vec_i >= vectors.size()) {
				// overflow
				actual = actual_end = av.end();
				break;
			}
		}

		// resolve total end
		size_t offset_in_actual = start - offset;
		size_t end_vec_i = actual_vec_i;
		while (true) {
			auto & av = *vectors[end_vec_i];
			if (size > av.size() - offset_in_actual) {
				// end in some next vector
				end_vec_i++;
				offset_in_actual = 0;
				size -= av.size();
				if (end_vec_i >= vectors.size()) {
					// overflow
					total_end = av.end();
					break;
				}
			} else {
				// end in this vector
				total_end = av.begin() + offset_in_actual + size;
				break;
			}
		}
	}

	ChaindedIter(const ChaindedIter & other,
			typename std::vector<T>::iterator actual_pos,
			typename std::vector<T>::iterator actual_end, size_t actual_vec_i) :
			vectors(other.vectors), actual(actual_pos), actual_end(actual_end), total_end(
					other.total_end), actual_vec_i(actual_vec_i) {
	}

	T& operator*() {
		return *actual;
	}

	T* operator->() {
		return &(*actual);
	}

	ChaindedIter& operator++() {
		actual++;
		if (actual == actual_end and actual != total_end) {
			actual_vec_i++;
			auto & av = *vectors[actual_vec_i];
			actual = av.begin();
			actual_end = av.end();
		}
		return *this;
	}

	ChaindedIter begin() const {
		return ChaindedIter(*this, actual, actual_end, actual_vec_i);
	}

	ChaindedIter end() const {
		return ChaindedIter(*this, total_end, actual, vectors.size());
	}

	friend bool operator==(ChaindedIter a, ChaindedIter b) {
		return a.actual == b.actual;
	}

	friend bool operator!=(ChaindedIter a, ChaindedIter b) {
		return a.actual != b.actual;
	}
};

}
}
