#pragma once
#include <taskflow/taskflow.hpp>

// based on https://github.com/robzan8/prefixsum/blob/master/src/prefixSumRec.hpp
// [TODO] use c++17 inclusive_scan after implemented also in GCC https://en.cppreference.com/w/cpp/algorithm/inclusive_scan

namespace netlistDB {
namespace parallel_utils {

/*
 * Simple serial version
 * */
template<typename T>
void inclusive_scan(T in[], T out[], size_t len) {
	if (len <= 0) {
		return;
	}

	out[0] = in[0];
	for (size_t i = 1; i < len; i++) {
		out[i] = out[i - 1] + in[i];
	}
}

/*
 * Perform prefix sum of item from array "in"
 * and store the result in to array "out"
 *
 * @param in the array with the input
 * @param out the array with the result
 * @param tmp the tmp array
 * @param len size of all arrays
 * @param tf the instance of TaskFlow thread pool
 * @param worker_threshold minimum amount of work for worker
 *         (if there is not enought work less worker thread is used)
 *
 * [TODO] Replaced with std/tbb impl.
 *   Looks like fork bomb but due TaskFlow load balancing scales relatively well.
 * */
template<typename T>
void inclusive_scan(T in[], T out[], T tmp[], size_t len, tf::Taskflow & tf, size_t min_work_per_thread = 1) {
	if (len <= min_work_per_thread) {
		inclusive_scan<T>(in, out, len);
		return;
	}
	size_t workers = tf.num_workers();
	size_t len_half = len / 2;
	T *in2 = out; // we can safely use "out" as tmp to keep "in2"
	T *out2 = tmp;

	auto w = std::min(workers, len_half);
	tf.parallel_for(0, int(w), 1, [=](int thread_i) {
		size_t first = thread_i * len_half / w;
		size_t end = (thread_i+1) * len_half / w;

		for (size_t j = first; j < end; j++) {
			in2[j] = in[j*2] + in[j*2+1];
		}
	});
	tf.wait_for_all();

	inclusive_scan(in2, out2, tmp + len_half, len_half, tf, min_work_per_thread);

	out[0] = in[0];
	size_t len1 = len - 1;
	w = std::min(workers, len1);
	tf.parallel_for(0, int(w), 1, [=](int i) {
		size_t first = 1 + i*len1/w;
		size_t end = 1 + (i+1)*len1/w;

		for (size_t j = first; j < end; j++) {
			if (j%2 == 0) {
				out[j] = out2[j/2-1] + in[j];
			} else {
				out[j] = out2[j/2];
			}
		}
	});
	tf.wait_for_all();
}

}
}
