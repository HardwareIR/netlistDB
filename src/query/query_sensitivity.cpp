#include <netlistDB/query/query_sensitivity.h>
#include <netlistDB/query/expr_sensitivity_probe.h>

using namespace std;

namespace netlistDB {
namespace query {

void QuerySensitivity::apply(Netlist & ctx) {
	set<iNode*> seen;
	for (auto n : ctx.nodes) {
		auto s = dynamic_cast<Statement*>(n);
		if (s) {
			apply(*s, seen);
			seen.clear();
		}
	}
}

void QuerySensitivity::apply(Statement & stm, set<iNode*> & seen) {
	auto a = dynamic_cast<Assignment*>(&stm);
	if (a) {
		apply(*a, seen);
		return;
	}
	auto i = dynamic_cast<IfStatement*>(&stm);
	if (i) {
		apply(*i, seen);
		return;
	}
	throw runtime_error("QuerySensitivity::apply not implemented for this data type");
}

void QuerySensitivity::apply(Assignment & assig, set<iNode*> & seen) {
	set<Net*> casualSensitivity;
	for (Net* inp : assig._inputs) {
		auto exit_inp = seen.find(inp);
		if (exit_inp == seen.end()) {
			seen.insert(inp);
			ExprSensitivityProbe::apply(*inp, casualSensitivity, seen,
					assig.sens.sensitivity);
		}
	}
	assig.sens.sensitivity.extend(casualSensitivity);
}

void QuerySensitivity::apply(IfStatement & ifstm, set<iNode*> & seen) {
	auto & ctx = ifstm.sens.sensitivity;

	ExprSensitivityProbe::apply(*ifstm.condition, seen, ctx);
	if (ctx.contains_event_dep) {
		ifstm.sens.is_completly_event_dependent = true;
		ifstm.sens.now_is_event_dependent = true;
		return;
	}

	for (auto stm : ifstm.ifTrue) {
		apply(*stm, seen);
		ctx.extend(stm->sens.sensitivity);
	}

	// else ifs
	for (auto & c : ifstm.elseIf) {
		Net *cond = c.first;
		auto & stms = c.second;
		if (ctx.contains_event_dep)
			break;

		ExprSensitivityProbe::apply(*cond, seen, ctx);
		if (ctx.contains_event_dep) {
			ifstm.sens.now_is_event_dependent = true;
			break;
		}

		for (auto stm : stms) {
			if (ctx.contains_event_dep)
				break;

			apply(*stm, seen);
			ctx.extend(stm->sens.sensitivity);
		}
	}
	if (ifstm.ifFalse_specified) {
		if (ifstm.sens.now_is_event_dependent)
			runtime_error(
					"can not negate use else branch together with condition with event expr.");
		// else
		for (auto stm : ifstm.ifFalse) {
			apply(*stm, seen);
			ctx.extend(stm->sens.sensitivity);
		}
	}
}

}
}
