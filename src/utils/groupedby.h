#pragma once

#include <map>
#include <vector>
#include <functional>

namespace netlistDB {
namespace utils {

/*
 * Group items from input by key specified
 * */
template<typename KEY_t, typename ITEM_t>
std::map<KEY_t, std::vector<ITEM_t>> groupedby(
		const std::vector<ITEM_t> & items,
		std::function<KEY_t(ITEM_t)> selector) {
	std::map<KEY_t, std::vector<ITEM_t>> m;
	for (auto i : items) {
		KEY_t k = selector(i);
		auto f = m.find(k);
		if (f != m.end()) {
			f->second.push_back(i);
		} else {
			m[k] = {i};
		}
	}
	return m;

}
}
}
