#include <netlistDB/transform/statement_to_hwprocess.h>
#include <netlistDB/transform/reduce_statement.h>
#include <netlistDB/utils/ordered_set.h>
#include <netlistDB/query/query_enclosure.h>
#include <netlistDB/query/query_sensitivity.h>
#include <netlistDB/query/query_public_net.h>
#include <netlistDB/query/query_structural_compatible.h>
#include <netlistDB/parallel_utils/erase_if.h>
#include <netlistDB/utils/groupedby.h>

using namespace std;
using namespace netlistDB::utils;
using namespace netlistDB::query;
using namespace netlistDB::parallel_utils;

namespace netlistDB {
namespace transform {

std::string TransformStatementToHwProcess::name_for_process(
		const OrderedSet<Net*> outputs) {
	for (auto o : outputs)
		if (not o->id.hidden and o->id.name.size() > 0)
			return "diver_of_" + o->id.name;

	return "proc";
}

Statement * TransformStatementToHwProcess::cut_off_drivers_of(Statement & self,
		Net* sig) {
	auto i = dynamic_cast<IfStatement*>(&self);
	if (i)
		return cut_off_drivers_of(*i, sig);

	throw runtime_error("not implement for statement of this type");
}
Statement * TransformStatementToHwProcess::cut_off_drivers_of(
		IfStatement & self, Net* sig) {
	if (self._outputs.size() == 1 and self._outputs[0] == sig) {
		// the net is only output of this statement so whole statement is cut off
		self.parent = nullptr;
		return &self;
	}

	// try to cut off all statements which are drivers of specified signal
	// in all branches

	vector<Statement*> newIfTrue;
	bool all_cut_off = true;
	all_cut_off &= cut_off_drivers_of(sig, self.ifTrue, newIfTrue);

	IfStatement::elseif_t newElifs;
	bool anyElifHit = false;
	for (auto & c : self.elseIf) {
		auto cond = c.first;
		auto & stms = c.second;
		vector<Statement*> newCase;
		all_cut_off &= cut_off_drivers_of(sig, stms, newCase);

		if (newCase.size())
			anyElifHit = true;

		newElifs.push_back( { cond, newCase });
	}
	vector<Statement*> newIfFalse;
	if (self.ifFalse.size()) {
		all_cut_off &= cut_off_drivers_of(sig, self.ifFalse, newIfFalse);
	}
	assert(
			not all_cut_off
					&& "everything was cut of but this should be already known at start");

	if (newIfTrue.size() or newIfFalse.size() or anyElifHit
			or newIfFalse.size()) {
		// parts were cut off
		// generate new statement for them
		Net * cond_sig = self.condition;
		auto n = new IfStatement(*cond_sig);
		(*n)(newIfTrue);
		for (auto & c : newElifs) {
			n->Elif(*c.first, c.second);
		}

		if (self.ifFalse_specified)
			n->Else(newIfFalse);

		if (self.parent == nullptr) {
			auto & ctx = n->_get_context();
			ctx.register_node(*n);
		}

		auto & inputs = self._inputs;
		// update io of this
		inputs.clear();
		inputs.push_back(cond_sig);
		for (auto & c : self.elseIf)
			self._inputs.push_back(c.first);

		self._inputs.push_back(cond_sig);
		self._outputs.clear();

		for (auto stm : self._iter_stms()) {
			for (auto inp : stm->_inputs)
				inputs.push_back(inp);

			for (auto outp : stm->_outputs)
				self._outputs.push_back(outp);
		}
		if (self.sens.sensitivity.size() or self.sens.enclosed_for.size()) {
			throw runtime_error(
					"Sensitivity and enclosure has to be cleaned first");
		}
		return n;
	}
	return nullptr;
}

void TransformStatementToHwProcess::apply(
		const std::vector<Statement*> & _statements,
		std::vector<HwProcess*> & res, bool try_solve_comb_loops) {
	assert(_statements.size());
	// try to simplify statements
	std::vector<Statement*> proc_statements;
	proc_statements.reserve(_statements.size() * 2);
	for (auto _stm : _statements) {
		TransformReduceStatement::apply(*_stm, proc_statements);
	}

	// collect IO and sensitivity
	OrderedSet<Net*> outputs;
	OrderedSet<Net*> _inputs;
	OrderedSet<iNode*> sensitivity;
	set<Net*> enclosed_for;
	for (auto _stm : proc_statements) {
		set<iNode*> seen;
		QuerySensitivity::apply(*_stm, seen);
		QueryEnclosure::apply(*_stm);
		outputs.extend(_stm->_outputs);
		_inputs.extend(_stm->_inputs);
		sensitivity.extend(_stm->sens.sensitivity);
		auto & ef = _stm->sens.enclosed_for;
		enclosed_for.insert(ef.begin(), ef.end());
	}

	// collect nop values for the signals
	std::map<Net*, Net*> enclosure_values;
	for (Net * net : outputs) {
		// inject nopVal if needed
		if (net->nop_val != nullptr) {
			enclosure_values[net] = net->nop_val;
		}
	}
	// fill up with nop values if required
	if (enclosure_values.size() > 0) {
		OrderedSet<Net*> do_enclose_for;
		for (Net * o : outputs) {
			auto f = enclosure_values.find(o);
			if (f != enclosure_values.end()) {
				do_enclose_for.push_back(o);
			}
		}
		fill_stm_list_with_enclosure(nullptr, enclosed_for, proc_statements,
				do_enclose_for, enclosure_values);
	}

	if (proc_statements.size()) {
		for (Net* o : outputs) {
			assert(not o->id.hidden);
		}
		// discover inputs (hidden signals are inside the expressions and are skipped)
		OrderedSet<Net*> inputs;
		{
			set<Net*> seen;
			for (Net *i : _inputs)
				QueryPublicNet::walk(*i, seen,
						[&inputs](Net &n) {inputs.push_back(&n);});
		}

		// check if there is a combinational loop (the output is direct input)
		set<Net*> intersect;
		set_intersection(outputs.begin(), outputs.end(), sensitivity.begin(),
				sensitivity.end(), std::inserter(intersect, intersect.begin()));
		if (intersect.size()) {
			if (not try_solve_comb_loops) {
				throw runtime_error(string("Combinational loop"));
			}
			// try to solve combinational loops by separating drivers of signals
			// from statements
			std::vector<Statement*> proc_stms_select;
			for (Net * net : intersect) {
				cut_off_drivers_of(net, proc_statements, proc_stms_select);
				apply(proc_stms_select, res, false);
				proc_stms_select.clear();
			}
			if (proc_statements.size())
				apply(proc_statements, res, false);
		} else {
			// the group of the statements is in proper format and the process can be generated
			auto p = new HwProcess(name_for_process(outputs), proc_statements,
					sensitivity, inputs, outputs);
			res.push_back(p);
		}
	} else {
		assert(outputs.size() == 0);
		// this can happen e.g. when "if statement" does not contains any Assignment
	}
}

void TransformStatementToHwProcess::clean_signal_meta(Statement & self) {
	auto i = dynamic_cast<IfStatement*>(&self);
	if (i) {
		i->sens_extra.ifTrue_enclosed_for.clear();
		i->sens_extra.elseIf_enclosed_for.clear();
		i->sens_extra.ifFalse_enclosed_for.clear();
	}

	self.sens.enclosed_for.clear();
	self.sens.sensitivity.clear();
	for (auto stm : self._iter_stms())
		clean_signal_meta(*stm);
}

bool TransformStatementToHwProcess::cut_off_drivers_of(Net * dstSignal,
		std::vector<Statement*> & statements,
		std::vector<Statement*> & separated) {

	std::vector<bool> stm_filter;
	stm_filter.reserve(statements.size());

	for (auto stm : statements) {
		clean_signal_meta(*stm);
		Statement * d = cut_off_drivers_of(*stm, dstSignal);
		if (d)
			separated.push_back(d);

		bool f = d == stm;
		stm_filter.push_back(f);
	}
	bool all_cut_off = true;
	for (size_t i = 0; i < stm_filter.size(); i++)
		if (stm_filter[i]) {
			statements.erase(statements.begin() + i);
		} else {
			all_cut_off = false;
		}
	return all_cut_off;

}

void TransformStatementToHwProcess::fill_enclosure(Statement & self,
		std::map<Net*, Net*> enclosure) {
	auto i = dynamic_cast<IfStatement*>(&self);
	if (i)
		return fill_enclosure(*i, enclosure);

	throw runtime_error("not implemented for statement of this type");
}

void TransformStatementToHwProcess::fill_enclosure(IfStatement & self,
		std::map<Net*, Net*> enclosure) {
	OrderedSet<Net*> enc;
	auto & outputs = self._outputs;
	for (auto e : enclosure) {
		if (outputs.contains(e.first)
				and not self.sens.enclosed_for.contains(e.first))
			enc.push_back(e.first);
	}
	if (enc.size() == 0)
		return;

	fill_stm_list_with_enclosure(&self, self.sens_extra.ifTrue_enclosed_for,
			self.ifTrue, enc, enclosure);

	auto e = self.sens_extra.elseIf_enclosed_for.begin();
	for (auto & c : self.elseIf) {
		fill_stm_list_with_enclosure(&self, *e, c.second, enc, enclosure);
		e++;
	}

	fill_stm_list_with_enclosure(&self, self.sens_extra.ifFalse_enclosed_for,
			self.ifFalse, enc, enclosure);

	self.sens.enclosed_for.extend(enc);
}

void TransformStatementToHwProcess::fill_stm_list_with_enclosure(
		Statement * parentStm, set<Net*> current_enclosure,
		vector<Statement*> statements, OrderedSet<Net*> do_enclose_for,
		map<Net*, Net*> enclosure) {
	for (auto e_sig : do_enclose_for) {
		{
			auto f = current_enclosure.find(e_sig);
			if (f != current_enclosure.end())
				continue;
		}
		bool enclosed = false;
		for (auto stm : statements) {
			if (stm->_outputs.contains(e_sig)) {
				if (not stm->sens.enclosed_for.contains(e_sig)) {
					fill_enclosure(*stm, enclosure);
				}
				enclosed = true;
				break;
			}
		}
		// any statement was not related with this signal,
		if (not enclosed) {
			Net * e = enclosure[e_sig];
			auto a = new Assignment(*e_sig, *e);
			statements.push_back(a);

			if (parentStm != nullptr)
				a->_set_parent_stm(parentStm);
		}
	}
}

size_t TransformStatementToHwProcess::max_stm_id(Statement & stm) {
	size_t maxId = 0;
	auto a = dynamic_cast<Assignment*>(&stm);
	if (a)
		return a->index;

	for (auto _stm : stm._iter_stms())
		maxId = std::max(maxId, max_stm_id(*_stm));
	return maxId;
}

size_t TransformStatementToHwProcess::max_stm_id(HwProcess & proc) {
	size_t maxId = 0;
	for (auto stm : proc.statements)
		maxId = std::max(maxId, max_stm_id(*stm));
	return maxId;
}

void TransformStatementToHwProcess::reduce_processes(
		std::vector<HwProcess*> & processes) {
	std::vector<HwProcess*> res;

	//processes.sort(key=lambda x: (x.name, maxStmId(x)), reverse=True)
	// now try to reduce processes with nearly same structure of statements into one
	// to minimize number of processes
	for (auto & g : groupedby<size_t, HwProcess*>(processes,
			[](HwProcess* p) {return p->rank;})) {
		auto & procs = g.second;

		// sort to make order of merging same deterministic
		std::sort(procs.begin(), procs.end(), [](HwProcess* a, HwProcess* b) {
			if (a->name == b->name) {
				// we are using the instance number of the first assignment
				// to make the order to be deterministic and natural
				return max_stm_id(*a) > max_stm_id(*b);
			} else {
				return a->name > b->name;
			}
		});

		for (size_t iA = 0; iA < procs.size(); iA++) {
			auto pA = procs[iA];
			if (pA == nullptr) {
				continue;
			}
			for (size_t iB = iA + 1; iB < procs.size(); iB++) {
				auto pB = procs[iB];
				if (pB == nullptr)
					continue;

				try {
					pA = tryToMerge(*pA, *pB);
				} catch (const IncompatibleStructure & e) {
					continue;
				}

				procs[iB] = nullptr;
			}
		}
		for (auto p : procs) {
			if (p != nullptr)
				res.push_back(p);
		}
	}
}

bool TransformStatementToHwProcess::checkIfIsTooSimple(HwProcess & proc) {
	if (proc.statements.size() != 1)
		return false;
	auto a = dynamic_cast<Assignment*>(proc.statements[0]);
	return bool(a);
}

bool does_intersect(const utils::OrderedSet<Net*> & a,
		const utils::OrderedSet<iNode*> & b) {
	return (reinterpret_cast<const utils::OrderedSet<iNode*>*>(&a))->does_intersects(b);
}

HwProcess * TransformStatementToHwProcess::tryToMerge(HwProcess & procA,
		HwProcess & procB) {
	if (checkIfIsTooSimple(procA) or checkIfIsTooSimple(procB)
			or does_intersect(procA.outputs, procB.sensitivityList)
			or does_intersect(procB.outputs, procA.sensitivityList)
			or not QueryStructuralComapatible::is_mergable(procA.statements,
					procB.statements)) {
		throw IncompatibleStructure();
	}

	TransformReduceStatement::merge_statements_vector(procA.statements,
			procB.statements);

	procA.outputs.extend(procB.outputs);
	procA.inputs.extend(procB.inputs);
	procA.sensitivityList.extend(procB.sensitivityList);

	return &procA;
}

}
}
