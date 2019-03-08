#pragma once
#include <vector>
#include <functional>
#include <algorithm>

namespace netlistDB {
namespace utils {

///*
// * The body of inclusive scan operation for tbb
// **/
//template<typename T>
//class SumBody {
//public:
//	SumBody(T y[], const T x[]) :
//			sum_(0), y_(y), x_(x) {
//	}
//	T get_sum() const {
//		return sum_;
//	}
//
//	template<typename Tag>
//	void operator()(const tbb::blocked_range<unsigned>& r, Tag) {
//		T temp = sum_;
//		for (unsigned i = r.begin(); i < r.end(); i++) {
//			temp += x_[i];
//			if (Tag::is_final_scan()) // Used to indicate that the initial scan is being performed
//				y_[i] = temp;
//		}
//		sum_ = temp;
//	}
//
//	// Split b so that this and b can accumulate separately.
//	SumBody(SumBody&b, tbb::split) :
//			sum_(0), y_(b.y_), x_(b.x_) {
//	}
//	// Merge preprocessing state of a into this, where a was created earlier
//	// from b by b¡¯s splitting constructor.
//	// The operation reverse_join is similar to the operation join
//	// used by parallel_reduce, except that the arguments are reversed
//	void reverse_join(SumBody& a) {
//		sum_ += a.sum_;
//	}
//	// Assign state of b to this.
//	void assign(SumBody& b) {
//		sum_ = b.sum_;
//	}
//private:
//	T sum_;
//	T* const y_;
//	const T* const x_;
//};
//
//template<typename T>
//struct GenerateIndex {
//	T* input;
//	unsigned * index;
//	std::function<bool(T)> predicate;
//	GenerateIndex(T* input, unsigned * index, std::function<bool(T)> predicate) :
//			input(input), index(index), predicate(predicate) {
//	}
//	void operator()(const tbb::blocked_range<unsigned>& range) const {
//		for (unsigned i = range.begin(); i != range.end(); ++i) {
//			index[i] = unsigned(not predicate(input[i]));
//		}
//	}
//};
//
//template<typename T, typename index_selector>
//struct PutItemOnIndex {
//	unsigned * index;
//	T* input;
//	T* output;
//	PutItemOnIndex(unsigned * index, T* input, T* output) :
//			index(index), input(input), output(output) {
//	}
//	void operator()(const tbb::blocked_range<unsigned>& range) const {
//		for (unsigned i = range.begin(); i != range.end(); ++i) {
//			if (index[i - 1] < index[i]) {
//				// put item on place and assign the index
//				auto n = input[i];
//				auto new_i = index[i] - 1;
//				index_selector { }(n) = new_i;
//				output[new_i] = n;
//			}
//		}
//	}
//
//};

template<typename T>
void erase_if_seq(std::vector<T> & vec, std::function<bool(T)> pred) {
	vec.erase(std::remove_if(vec.begin(), vec.end(), pred), vec.end());
}

template<typename T, typename index_selector>
void erase_if_seq(std::vector<T> & vec, std::function<bool(T)> pred) {
	erase_if_seq<T>(vec, pred);
	size_t i = 0;
	for (T & item : vec) {
		index_selector { }(item) = i;
		i++;
	}
}

template<typename T, typename index_selector>
void erase_if(std::vector<T> & vec, std::function<bool(T)> pred) {
	erase_if_seq<T, index_selector>(vec, pred);
}
/*
 * @param pred predicate returns true if item should be erased
 *
 * [TODO] add check if none will be removed do not perform scanning, reallocations and copy
 **/
//template<typename T, typename index_selector>
//void erase_if(std::vector<T> & vec, std::function<bool(T)> pred) {
//	size_t item_cnt = vec.size();
//	if (item_cnt == 0)
//		return;
//	auto index_in = std::make_unique<unsigned[]>(item_cnt);
//	//auto index_tmp = std::make_unique<unsigned[]>(item_cnt);
//	auto index_out = std::make_unique<unsigned[]>(item_cnt + 1);
//
//	GenerateIndex<T> index_gen(&vec[0], index_in.get(), pred);
//	tbb::parallel_for(tbb::blocked_range<unsigned>(0, item_cnt), index_gen);
//
//	// now index_in is filled with 1 for every item which should stay and 0 for each item to remove
//	SumBody<unsigned> body(index_out.get(), index_in.get());
//	tbb::parallel_scan(tbb::blocked_range<unsigned>(0, item_cnt), body);
//
//	size_t new_item_cnt = index_out.get()[item_cnt - 1];
//	std::vector<T> new_vec(new_item_cnt);
//	// if index[i-1] < index[i] then item on index i should go in to output on index[i]
//	PutItemOnIndex<T, index_selector> itemPut(index_out.get(), &vec[0],
//			&new_vec[0]);
//	tbb::parallel_for(tbb::blocked_range<unsigned>(1, item_cnt), itemPut);
//
//	// first item processed separately to simplify expression
//	if (index_out[0] == 1) {
//		new_vec[0] = vec[0];
//		index_selector { }(new_vec[0]) = 0;
//	}
//	//tf.wait_for_all();
//
//	vec = new_vec;
//}
}
}
