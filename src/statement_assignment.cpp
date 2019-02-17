#include "statement_assignment.h"
#include "operator_defs.h"

namespace netlistDB {

void init_iterators(Assignment * a) {
	a->forward.push_back(reinterpret_cast<iNode*>(&a->dst));
	a->backward.push_back(reinterpret_cast<iNode*>(&a->src));
	a->backward.push_back(
			reinterpret_cast<std::vector<iNode*>*>(&a->dst_index));
}

Assignment::Assignment(Net & dst, Net & src) :
		dst(extract_dst_index_cascade(dst)), src(src) {
	this->dst.ctx.register_node(*this);
	this->dst.drivers.push_back(this);
	src.endpoints.push_back(this);
	for (auto i : dst_index) {
		i->endpoints.push_back(this);
	}
	init_iterators(this);
}

//Assignment::Assignment(Net & dst, std::initializer_list<Net*> dst_index,
//		Net & src) :
//		dst_index(dst_index),
//		dst(extract_dst_index_cascade(dst)), src(src) {
//	dst.ctx.register_node(*this);
//	dst.drivers.push_back(this);
//	for (auto i : dst_index) {
//		i->endpoints.push_back(this);
//	}
//	src.endpoints.push_back(this);
//	init_iterators(this);
//}

Net & Assignment::extract_dst_index_cascade(Net & dst) {
	// now I am result of the index  xxx[xx] <= source
	// get index op
	if (dst.drivers.size() != 1)
		return dst;

	auto d = dst.drivers[0];
	auto op = dynamic_cast<FunctionCall*>(d);

	if (op != nullptr and &op->fn == &OpSlice) {
		// get signal on which is index applied
		Net * indexed_on = op->args[0];
		dst_index.push_back(op->args[1]);
		return extract_dst_index_cascade(*indexed_on);
	}
	return dst;
}

}
