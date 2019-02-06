#pragma once
#include <vector>
#include <functional>

namespace netlistDB {
namespace parallel_utils {

template<typename T>
typename std::vector<T>::iterator errase_if(std::vector<T> & vec,
		std::function<bool(T)> pred) {
	return vec.erase(std::remove_if(vec.begin(), vec.end(), pred), vec.end());
}

/*
 * Remove the nullptrs and regenerate index
 * */
template<typename T, typename index_selector>
void compress_vec(std::vector<T*> & vec) {
	vec.erase(std::remove_if(vec.begin(), vec.end(), [](T*i) {
		return i == nullptr;
	}), vec.end());
	size_t i = 0;
	for (auto item : vec) {
		index_selector { }(item) = i;
		i++;
	}
}

template<typename T>
void compress_vec(std::vector<T*> & vec) {
	vec.erase(std::remove_if(vec.begin(), vec.end(), [](T*i) {
		return i == nullptr;
	}), vec.end());
}

}
}
