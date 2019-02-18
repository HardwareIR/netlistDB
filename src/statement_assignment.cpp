#include <netlistDB/statement_assignment.h>
#include <netlistDB/operator_defs.h>
#include <netlistDB/query/expr_sensitivity_probe.h>

using namespace std;
namespace netlistDB {

Assignment::Assignment(Net & _dst, Net & _src) :
		Statement(), dst(extract_dst_index_cascade(_dst)), src(_src) {
	dst.ctx.register_node(*this);
	dst.drivers.push_back(this);
	src.endpoints.push_back(this);
	for (auto i : dst_index) {
		i->endpoints.push_back(this);
		_inputs.push_back(i);
	}
	_inputs.push_back(&src);
	_outputs.push_back(&dst);

	forward.push_back(reinterpret_cast<vector<iNode*>*>(&_outputs));
	backward.push_back(reinterpret_cast<iNode*>(&src));
	backward.push_back(reinterpret_cast<vector<iNode*>*>(&dst_index));
}

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

utils::ChainedSequence<Statement*> Assignment::_iter_stms() {
	return {};
}

}
