#include "netlist.h"

namespace netlistDB {

Netlist & Statement::_get_context() {
	for (auto outp : forward)
		return dynamic_cast<Net*>(outp)->ctx;

	for (auto inp : backward)
		return dynamic_cast<Net*>(inp)->ctx;

	for (auto s: _iter_stms())
		return s->_get_context();

	throw std::runtime_error(
			"the statement is entirely disconnected, the context is lost");
}

void Statement::_on_parent_event_dependent() {
	if (not sens.is_completly_event_dependent) {
		sens.is_completly_event_dependent = true;
		for (auto stm : _iter_stms()) {
			stm->_on_parent_event_dependent();
		}
	}
}
void Statement::_set_parent_stm(Statement * stm) {
	bool was_top = parent == nullptr;
	parent = stm;
	if (not sens.now_is_event_dependent and stm->sens.now_is_event_dependent) {
		_on_parent_event_dependent();
	}

	auto topStatement = parent;
	while (topStatement->parent != nullptr) {
		topStatement = topStatement->parent;
	}

	if (was_top) {
		// disconnect IO from this statement
		// and connect it to top statement
		for (auto inp : _inputs) {
			inp->endpoints.discard(this);
			inp->endpoints.push_back(topStatement);
			topStatement->_inputs.push_back(inp);
		}
		for (auto outp : _outputs) {
			outp->drivers.discard(this);
			outp->drivers.push_back(topStatement);
			topStatement->_outputs.push_back(outp);
		}
		// unregister this statement from nodes as this
		// will be stored in some other statement
		Netlist & ctx = _get_context();
		ctx.unregister_node(*this);
	}
	parent->rank += rank;
}

void Statement::_register_stements(const std::vector<Statement*> & statements,
		std::vector<Statement*> & target) {
	target.reserve(target.size() + statements.size());
	for (auto stm : statements) {
		assert(stm->parent == nullptr);
		stm->_set_parent_stm(this);
		target.push_back(stm);
	}
}

}
