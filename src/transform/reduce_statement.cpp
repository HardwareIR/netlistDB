#include "reduce_statement.h"
#include "../utils/groupedby.h"
#include "../query/query_structural_compatible.h"

#include <tuple>

using namespace std;
using namespace netlistDB::utils;

namespace netlistDB {
namespace transform {

bool TransformReduceStatement::apply(Statement & stm,
		vector<Statement*> & res) {
	auto * a = dynamic_cast<Assignment*>(&stm);
	if (a)
		return apply(*a, res);

	auto * i = dynamic_cast<IfStatement*>(&stm);
	if (i)
		return apply(*i, res);

	throw runtime_error("unknown statement type");
}

bool TransformReduceStatement::apply(Assignment & stm,
		vector<Statement*> & res) {
	res.push_back(&stm);
	return false;
}

void TransformReduceStatement::merge_with_other_stm(Statement & a,
		Statement & b) {
	auto * ia = dynamic_cast<IfStatement*>(&a);
	auto * ib = dynamic_cast<IfStatement*>(&b);
	if (ia and ib)
		return merge_with_other_stm(*ia, *ib);

	throw runtime_error("can not merge statements of this type");
}

void TransformReduceStatement::merge_with_other_stm(IfStatement & self,
		IfStatement & other) {
	self.ifTrue = merge_statements_vector(self.ifTrue, other.ifTrue);

	vector<pair<Net*, vector<Statement*>>> new_elifs;
	new_elifs.reserve(self.elseIf.size());

	assert(self.elseIf.size() == other.elseIf.size());
	auto b_el = other.elseIf.begin();
	for (auto & a : self.elseIf) {
		new_elifs.push_back(
				{ a.first, merge_statements_vector(a.second, b_el->second) });
		b_el++;
	}
	self.elseIf = new_elifs;

	self.ifFalse = merge_statements_vector(self.ifFalse, other.ifFalse);

	other.ifTrue.clear();
	other.elseIf.clear();
	other.ifFalse.clear();
	other.ifTrue_specified = false;
	other.ifFalse_specified = false;

	on_merge(self, other);
}

vector<Statement*> TransformReduceStatement::merge_statements_vector(
		vector<Statement*> & stmsA, vector<Statement*> & stmsB) {
	vector<Statement*> tmp;

	auto a_it = stmsA.begin();
	auto b_it = stmsB.begin();

	Statement * a = nullptr;
	Statement * b = nullptr;
	bool a_empty = false;
	bool b_empty = false;

	while (not a_empty and not b_empty) {
		while (not a_empty) {
			if (a_it == stmsA.end()) {
				a = nullptr;
			} else {
				a = *a_it;
				a_it++;
			}
			if (a == nullptr) {
				a_empty = true;
				break;
			} else if (a->rank == 1) {
				// simple statement does not require merging
				tmp.push_back(a);
				a = nullptr;
			} else {
				break;
			}
		}
		while (not b_empty) {
			if (b_it == stmsB.end()) {
				b = nullptr;
			} else {
				b = *b_it;
				b_it++;
			}
			if (b == nullptr) {
				b_empty = true;
				break;
			} else if (b->rank == 1) {
				// simple statement does not require merging
				tmp.push_back(b);
				b = nullptr;
			} else {
				break;
			}
		}
		if (a != nullptr or b != nullptr) {
			if (a == nullptr) {
				a = b;
				b = nullptr;
			}
			if (a and b)
				merge_with_other_stm(*a, *b);
			tmp.push_back(a);
			a = nullptr;
			b = nullptr;
		}
	}
	return tmp;
}

size_t TransformReduceStatement::merge_statements(
		const vector<Statement*> & statements,
		vector<Statement*> & new_statements) {
	map<const Statement*, size_t> order;
	size_t i = 0;
	for (auto stm : statements) {
		order[stm] = i++;
	}

	size_t rank_decrease = 0;
	auto new_begin = new_statements.end();

	for (auto & itm : groupedby<size_t, Statement*>(statements,
			[](Statement* s) {return s->rank;})) {
		size_t rank = itm.first;
		vector<Statement*> & stms = itm.second;
		if (rank == 0) {
			copy(stms.begin(), stms.end(), back_inserter(new_statements));
		} else {
			if (stms.size() == 1) {
				new_statements.push_back(stms[0]);
				continue;
			}

			// try to merge statements if they are same condition tree
			for (size_t iA = 0; iA < stms.size(); iA++) {
				auto stmA = stms[iA];
				if (stmA == nullptr)
					continue;

				for (size_t iB = iA + 1; iB < stms.size(); iB++) {
					auto stmB = stms[iB];
					if (stmB == nullptr) {
						continue;
					}

					if (query::QueryStructuralComapatible::is_mergable(*stmA,
							*stmB)) {
						rank_decrease += stmB->rank;
						merge_with_other_stm(*stmA, *stmB);
						stms[iB] = nullptr;
						new_statements.push_back(stmA);
					} else {
						new_statements.push_back(stmA);
						new_statements.push_back(stmB);
					}
				}
			}
		}
	}
	sort(new_begin, new_statements.end(),
			[order](const Statement * a, const Statement * b) -> bool
			{
				return order.find(a)->second > order.find(b)->second;
			});
	return rank_decrease;

}

pair<size_t, bool> TransformReduceStatement::apply(
		vector<Statement*> & statements, vector<Statement*> & new_statements) {
	bool io_change = false;

	vector<Statement*> tmp;
	for (auto stm : statements) {
		bool _io_change = apply(*stm, tmp);
		io_change |= _io_change;
	}
	size_t rank_decrease = merge_statements(tmp, new_statements);

	return {rank_decrease, io_change};
}

void TransformReduceStatement::merge_nested_if_from_else(IfStatement & self,
		IfStatement & ifStm) {
	self.elseIf.push_back( { ifStm.condition, ifStm.ifTrue });
	copy(ifStm.elseIf.begin(), ifStm.elseIf.end(), back_inserter(self.elseIf));

	self.ifFalse = ifStm.ifFalse;
}

void TransformReduceStatement::collect_io(Statement & self) {
	auto & i = self._inputs;
	auto & o = self._outputs;

	for (auto stm : self._iter_stms()) {
		i.extend(stm->_inputs);
		o.extend(stm->_outputs);
	}
}

void TransformReduceStatement::on_reduce(Statement & stm, bool self_reduced,
		bool io_changed, std::vector<Statement*> & result_statements) {
	auto parentStm = stm.parent;
	if (self_reduced) {
		bool was_top = parentStm == nullptr;
		// update signal drivers/endpoints
		if (was_top) {
			// disconnect self from signals
			Netlist & ctx = stm._get_context();
			ctx.unregister_node(stm);
			for (auto s : result_statements) {
				ctx.register_node(*s);
			}

			for (auto i : stm._inputs)
				i->endpoints.discard(&stm);

			for (auto o : stm._outputs)
				o->drivers.discard(&stm);
		}
		for (auto s : result_statements) {
			s->parent = parentStm;
			if (parentStm == nullptr) {
				// connect signals to child statements
				for (auto i : s->_inputs)
					i->endpoints.push_back(s);
				for (auto o : s->_outputs)
					o->drivers.push_back(s);
			}
		}
	} else {
		// parent has to update it's inputs/outputs
		if (io_changed) {
			stm._inputs.clear();
			stm._outputs.clear();
			collect_io(stm);
		}
	}
}

bool TransformReduceStatement::apply(IfStatement & stm,
		vector<Statement*> & res) {
	// flag if IO of statement has changed and should be re-evaluated
	bool io_change = false;

	size_t rank_decrease;
	vector<Statement*> new_ifTrue;
	bool _io_change;
	tie(rank_decrease, _io_change) = apply(stm.ifTrue, new_ifTrue);
	stm.rank -= rank_decrease;
	io_change |= _io_change;
	stm.ifTrue = new_ifTrue;

	elseif_t new_elifs;
	for (auto & c : stm.elseIf) {
		Net *cond = c.first;
		auto & statements = c.second;
		vector<Statement*> new_statements;
		tie(rank_decrease, _io_change) = apply(statements, new_statements);
		stm.rank -= rank_decrease;
		io_change |= _io_change;
		new_elifs.push_back( { cond, new_statements });
	}
	stm.elseIf = new_elifs;

	if (stm.ifFalse.size()) {
		vector<Statement*> new_ifFalse;
		bool _io_update_required;
		tie(rank_decrease, _io_update_required) = apply(stm.ifFalse,
				new_ifFalse);
		stm.ifFalse = new_ifFalse;
		stm.rank -= rank_decrease;
		io_change |= _io_change;
	}
	bool reduce_self =
			not query::QueryStructuralComapatible::if_cond_has_effect(
					stm.ifTrue, stm.ifFalse, stm.ifFalse_specified, stm.elseIf);

	if (reduce_self) {
		copy(stm.ifTrue.begin(), stm.ifTrue.end(), back_inserter(res));
	} else {
		res.push_back(&stm);
	}

	on_reduce(stm, reduce_self, io_change, res);

	// try merge nested ifs as elifs
	if (stm.ifFalse.size() == 1) {
		auto child = stm.ifFalse[0];
		auto sub_if = dynamic_cast<IfStatement*>(child);
		if (sub_if) {
			merge_nested_if_from_else(stm, *sub_if);
		}
	}
	return io_change;
}

void TransformReduceStatement::on_merge(Statement & self, Statement & other) {
	self._inputs.extend(other._inputs);
	self._outputs.extend(other._outputs);

	self.sens.sensitivity.extend(other.sens.sensitivity);
	auto & e = other.sens.enclosed_for;
	self.sens.enclosed_for.extend(e);

	bool other_was_top = other.parent == nullptr;
	if (other_was_top) {
		other._get_context().unregister_node(other);
		for (auto s : other._inputs) {
			s->endpoints.discard(&other);
			s->endpoints.push_back(&self);
		}

		for (auto s : other._outputs) {
			s->drivers.discard(&other);
			s->drivers.push_back(&self);
		}
	}
}

}
}
