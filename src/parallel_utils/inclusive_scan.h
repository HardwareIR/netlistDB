#pragma once
#include <taskflow/taskflow.hpp>

// similar to https://github.com/robzan8/prefixsum/blob/master/src/prefixSumRec.hpp
// [TODO] use c++17 inclusive_scan after implemented also in GCC https://en.cppreference.com/w/cpp/algorithm/inclusive_scan

namespace netlistDB {
namespace parallel_utils {
template<typename T>
void inclusive_scan(T in[], T out[], T tmp[], size_t len, tf::Taskflow & tf) {
	if (len == 1) {
		out[0] = in[0];
		return;
	}
	size_t workers = tf.num_workers();
	size_t len_half = len / 2;
	T *in2 = out; // we can safely use "out" as tmp to keep "in2"
	T *out2 = tmp;

	auto w = std::min(workers, len_half);
	tf.parallel_for(0, w, [=](const size_t thread_i) {
		size_t first = thread_i * len_half / w;
		size_t end = (thread_i+1) * len_half / w;

		for (long j = first; j < end; j++) {
			in2[j] = in[j*2] + in[j*2+1];
		}
	});

	inclusive_scan(in2, out2, tmp + len_half, len_half, tf);

	out[0] = in[0];
	size_t len1 = len - 1;
	w = std::min(workers, len1);
	tf.parallel_for(0, w, [=](const size_t i) {
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
}

/*
 * Perform prefix sum of item from array "in"
 * and store the result in to array "out"
 *
 * @param in the array with the input
 * @param out the array with the result
 * @param tmp the tmp array
 * @param len size of all arrays
 * @param thread_cnt the max number of the worker threads to use
 * @param worker_threshold minimum amount of work for worker
 *         (if there is not enought work less worker thread is used)
 * */
template<typename T>
void inclusive_scan(T in[], T out[], T tmp[], size_t len, size_t thread_cnt,
		size_t worker_threshold = 1) {
	if (len <= 0 || thread_cnt <= 0) {
		return;
	}

	thread_cnt = std::min(thread_cnt, len / worker_threshold);

	if (thread_cnt == 1) {
		inclusive_scan<T>(in, out, len);
		return;
	}

	tf::Taskflow tf(thread_cnt);
	inclusive_scan(in, out, tmp, len, tf, thread_cnt);
}

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

}
}
