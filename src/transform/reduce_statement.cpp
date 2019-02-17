#include "reduce_stetement.h"

using namespace std;
namespace netlistDB {
namespace transform {

bool TransformReduceStatement::apply(Statement & stm,
		vector<Statement*> & res) {

}

bool TransformReduceStatement::apply(Assignment & stm,
		vector<Statement*> & res) {
	res.push_back(&stm);
	return false;
}

template<typename KEY_t, typename ITEM_t>
map<KEY_t, vector<ITEM_t>> groupedby(const vector<ITEM_t> & items, function<KEY_t(ITEM_t)> selector) {
	map<KEY_t, vector<ITEM_t>> m;
	for (auto i: items) {
		KEY_t k = selector(i);
		auto f = m.find(k);
		if (f != m.end()) {
			f.second.push_back(i);
		} else {
			m[k] = {i};
		}
	}
	return m;

}

size_t TransformReduceStatement::merge_statements(
		const vector<Statement*> & statements,
		vector<Statement*> & new_statements) {
	map<Statement*, size_t> order;
	size_t i = 0;
	for (auto stm : statements) {
		order[stm] = i++;
	}

	size_t rank_decrease = 0;
	auto new_begin = new_statements.end();

	for (auto & itm : groupedby<size_t, Statement*>(statements, [](Statement* s) {return s->rank;})) {
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

					if (is_mergable(stmA, stmB)) {
						rank_decrease += stmB->rank;
						merge_with_other_stm(stmA, stmB);
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
				return order[a] > order[b];
			});
	return rank_decrease;

}

//  Simplify statements in the vector
pair<size_t, bool> apply(vector<Statement*> statements,
		vector<Statement*> new_statements) {
	bool io_change = false

	vector<Statement*> tmp;
	for (auto stm : statements) {
		bool _io_change = apply(stm, tmp);
		io_change |= _io_change;
	}
	size_t rank_decrease = merge_statements(tmp, new_statements);

	return {rank_decrease, io_change};
}

bool TransformReduceStatement::apply(IfStatement & stm,
		vector<Statement*> & res) {
	// flag if IO of statement has changed and shoud be re-evaluated
	bool io_change = false;

	size_t rank_decrease;
	vector<Statement*> new_ifTrue;
	bool _io_change;
	tie(rank_decrease, _io_change) = apply(stm.ifTrue, new_ifTrue);
	stm.rank -= rank_decrease;
	io_change |= _io_change;
	stm.ifTrue = new_ifTrue;

	vector<pair<Net*, vector<Statement*>>> new_elifs;
	for (auto & c : stm.elseIf) {
		Net *cond = c.first;
		auto & statements = c.second;
		vector<Statement*> new_statements;
		rank_decrease, _io_change = apply(statements, new_statements);
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
	bool reduce_self = not if_cond_has_effect(stm.ifTrue, stm.ifFalse,
			stm.elseIf);

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
			merge_nested_if_from_else(stm, *child);
		}
	}
	return io_change;
}
}
}
