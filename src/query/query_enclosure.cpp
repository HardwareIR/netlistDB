#include "query_enclosure.h"

namespace netlistDB {
namespace query {

template<typename T, typename T_itterable>
bool not_in(T item, T_itterable & itr) {
	auto f = itr.find(item);
	return f == itr.end();
}

template<typename T, typename T_itterable>
bool is_in(T item, T_itterable & itr) {
	auto f = itr.find(item);
	return f != itr.end();
}

void QueryEnclosure::apply(Statement & stm) {
	auto a = dynamic_cast<Assignment*>(&stm);
	if (a) {
		apply(*a);
		return;
	}
	auto i = dynamic_cast<IfStatement*>(&stm);
	if (i) {
		apply(*i);
	}
	throw std::runtime_error("not implemented");
}

void QueryEnclosure::apply(Assignment & a) {
	a.sens.enclosed_for.extend(a._outputs);
}

/*
 * Discover enclosure for list of statements
 *
 * @param statements statements in one code branch
 * @param outputs outputs which should be driven from this statements
 * @return set of signals for which this statements have always some drive
 *     (is enclosed)
 * */
std::set<Net*> QueryEnclosure::apply(const std::vector<Statement*> & statements,
		const utils::OrderedSet<Net*> & outputs) {
	std::set<Net*> result;
	if (statements.size() == 0) {
		return result;
	}

	for (auto stm : statements)
		apply(*stm);

	for (auto o : outputs) {
		bool has_driver = false;

		for (auto stm : statements) {
			if (is_in(o, stm->_outputs)) {
				assert(not has_driver);
				has_driver = true;
				if (is_in(o, stm->sens.enclosed_for))
					result.insert(o);
			}
		}
	}
	return result;
}

/*
 * Discover the enclosure for if statement and all potential child statements
 * */
void QueryEnclosure::apply(IfStatement & ifstm) {
	auto & outputs = ifstm._outputs;
	ifstm.sens_extra.ifTrue_enclosed_for = QueryEnclosure::apply(ifstm.ifTrue,
			outputs);

	auto & elif_encls = ifstm.sens_extra.elseIf_enclosed_for;
	for (auto & c : ifstm.elseIf) {
		auto & stms = c.second;
		auto e = QueryEnclosure::apply(stms, outputs);
		elif_encls.push_back(e);
	}
	ifstm.sens_extra.ifFalse_enclosed_for = QueryEnclosure::apply(ifstm.ifFalse,
			outputs);

	auto & encl = ifstm.sens.enclosed_for;

	for (auto & s : ifstm.sens_extra.ifTrue_enclosed_for) {
		bool enclosed = true;

		// check if is driven also by all elifs
		for (auto & elif_e : elif_encls) {
			if (not_in(s, elif_e)) {
				enclosed = false;
				break;
			}
		}

		// if driven by elifs and ifFalse then is driven always
		if (enclosed and is_in(s, ifstm.sens_extra.ifFalse_enclosed_for)) {
			encl.push_back(s);
		}
	}
}

}
}
