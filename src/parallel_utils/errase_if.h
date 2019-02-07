#pragma once
#include <vector>
#include <functional>
#include <algorithm>
#include <taskflow/taskflow.hpp>
#include "inclusive_scan.h"

namespace netlistDB {
namespace parallel_utils {

template<typename T>
void erase_if(std::vector<T*> & vec, std::function<bool(T*)> pred) {
	vec.erase(std::remove_if(vec.begin(), vec.end(), pred), vec.end());
}

template<typename T, typename index_selector>
void erase_if(std::vector<T*> & vec, std::function<bool(T*)> pred) {
	erase_if<T>(vec, pred);
	size_t i = 0;
	for (T* item : vec) {
		index_selector { }(item) = i;
		i++;
	}
}

/*
 * @param pred predicate returns true if item should be erased
 **/
template<typename T, typename index_selector>
void erase_if(std::vector<T*> & vec, std::function<bool(T*)> pred,
		tf::Taskflow & tf) {
	size_t item_cnt = vec.size();
	auto index_in = std::make_unique<unsigned[]>(item_cnt);
	auto index_tmp = std::make_unique<unsigned[]>(item_cnt);
	auto index_out = std::make_unique<unsigned[]>(item_cnt + 1);
	// floor div
	size_t step = item_cnt / tf.num_workers();
	if (step * tf.num_workers() < item_cnt) {
		step++;
	}

	// initialize index
	for (size_t w = 0; w < tf.num_workers(); w++) {
		tf.silent_emplace(
				[w, index=index_in.get(), item_cnt, step, vec=&vec[0], pred]() {
					size_t last = std::min((w+1)*step, item_cnt);
					for (size_t i = w*step; i < last; i++) {
						// 1 if item should stay 0 if should be removed
						index[i] = unsigned(not pred(vec[i]));
					}
				});
	}
	tf.wait_for_all();

	// now index_in is filled with 1 for every item which should stay and 0 for each item to remove
	inclusive_scan<unsigned>(index_in.get(), index_out.get(), index_tmp.get(),
			item_cnt, tf);

	size_t new_item_cnt = index_out.get()[item_cnt - 1];
	std::vector<T*> new_vec(new_item_cnt);
	// if index[i-1] < index[i] then item on index i should go in to output on index[i]
	for (size_t w = 0; w < tf.num_workers(); w++) {
		tf.silent_emplace(
				[w, index=index_out.get(), item_cnt, step, &vec, new_vec=&new_vec[0]]() {
					size_t last = std::min((w+1)*step, item_cnt);
					// first item is skipped and processed separately
					size_t i = std::max(size_t(1), w*step);
					for (; i < last; i++) {
						if (index[i-1] < index[i]) {
							// put item on place and assign the index
							auto n = vec[i];
							auto new_i = index[i] - 1;
							index_selector {}(n) = new_i;
							new_vec[new_i] = n;
						}
					}
				});
	}
	// first item processed separately to simplify expression
	if (index_out[0] == 1) {
		new_vec[0] = vec[0];
		index_selector { }(new_vec[0]) = 0;
	}
	tf.wait_for_all();

	vec = new_vec;
}

}
}
