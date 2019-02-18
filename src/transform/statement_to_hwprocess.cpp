#include "statement_to_hwprocess.h"
#include "../utils/ordered_set.h"
#include "../query/query_enclosure.h"
#include "../query/query_sensitivity.h"

using namespace std;
using namespace netlistDB::utils;
using namespace netlistDB::query;

namespace netlistDB {
namespace transform {

void TransformStatementToHwProcess::apply(
		const std::vector<Statement*> & _statements,
		std::vector<HwProcess*> & res, bool try_solve_comb_loops) {
	assert(_statements.size());
	//// try to simplify statements
	//std::vector<Statement*> proc_statements;
	//proc_statements.reserve(_statements.size()*2);
	//for (auto _stm: _statements) {
	//    TransformReduceStatement::apply(_stm, proc_statements);
	//}
	//
	//OrderedSet<Net*> outputs;
	//OrderedSet<Net*> _inputs;
	//OrderedSet<iNode*> sensitivity;
	//set<Net*> enclosed_for;
	//for (auto _stm: proc_statements) {
	//    set<iNode*> seen;
	//    QuerySensitivity::apply(*_stm, seen);
	//    QueryEnclosure::apply(*_stm);
	//    outputs.extend(_stm->_outputs);
	//    _inputs.extend(_stm->_inputs);
	//    sensitivity.extend(_stm->sens.sensitivity);
	//    enclosed_for.update(_stm->sens.enclosed_for);
	//}
	//std::map<Net*, Net*> enclosure_values;
	//for (Net * net: outputs) {
	//    // inject nopVal if needed
	//    if (net->nop_val != nullptr) {
	//        enclosure_values[net] = net->nop_val;
	//    }
	//}
	//if (enclosure_values.size() > 0) {
	//    vector<Net*> do_enclose_for;
	//    for (Net * o: outputs) {
	//    	auto f = enclosure_values.find(o);
	//    	if (f != enclosure_values.end()) {
	//    		do_enclose_for.push_back(o);
	//    	}
	//    }
	//    fill_stm_list_with_enclosure(nullptr, enclosed_for, proc_statements,
	//                                 do_enclose_for, enclosure_values);
	//}
	//if (proc_statements.size()) {
	//    for (Net* o: outputs) {
	//        assert(not o->id.hidden);
	//    }
	//	set<iNode*> seen;
	//	OrderedSet<Net*> inputs;
	//    for (Net *i: _inputs)
	//        inputs.extend(i._walk_public_drivers(seen))
	//
	//    intersect = outputs.intersection_set(sensitivity)
	//    if (intersect.size()) {
	//        if (not try_solve_comb_loops) {
	//            throw runtime_error(string("Combinational loop on signal(s)") + std::to_string(intersect));
	//        }
	//        // try to solve combinational loops by separating drivers of signals
	//        // from statements
	//        for (Net * net: intersect) {
	//            proc_statements, proc_stms_select = cut_off_drivers_of(
	//                net, proc_statements)
	//            apply(proc_stms_select, false);
	//        }
	//        if (proc_statements.size())
	//            apply(proc_statements, false);
	//    } else {
	//        string name = name_for_process_and_mark_outputs(proc_statements);
	//        auto & p = HwProcess("assig_process_" + name,
	//                        proc_statements, sensitivity,
	//                        inputs, outputs);
	//        res.push_back(p);
	//    }
	//} else {
	//    assert(outputs.size() == 0);
	//    // this can happen e.g. when "if statement" does not contains any Assignment
	//}
}

}
}
