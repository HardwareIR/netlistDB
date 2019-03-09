#include <netlistDB/query/expr_sensitivity_probe.h>
#include <netlistDB/operator_defs.h>

using namespace std;
namespace netlistDB {
namespace query {

void ExprSensitivityProbe::apply(FunctionCall & fn_call,
		set<Net*> & casual_sensitivity, set<iNode*> & seen,
		utils::SensitivityCtx & ctx) {
	seen.insert(&fn_call);

	if (is_event_op(fn_call.fn)) {
		ctx.contains_event_dep = true;
		ctx.push_back(&fn_call);
	} else {
		// walk source of signal
		for (auto operand : fn_call.args) {
			auto op_seen = seen.find(operand);
			if (op_seen == seen.end()) {
				apply(*operand, casual_sensitivity, seen, ctx);
			}
		}
	}
}

void ExprSensitivityProbe::apply(Net & net, set<Net*> & casual_sensitivity,
		set<iNode*> & seen, utils::SensitivityCtx & ctx) {
	seen.insert(&net);

	if (net.is_const())
		return;

	FunctionCall * op = nullptr;
	if (net.drivers.size() == 1)
		op = dynamic_cast<FunctionCall*>(net.drivers[0]);

	if (op == nullptr) {
		casual_sensitivity.insert(&net);
		return;
	}

	apply(*op, casual_sensitivity, seen, ctx);
}

void ExprSensitivityProbe::apply(Net & net, std::set<iNode*> & seen,
		utils::SensitivityCtx & ctx) {
	set<Net*> casualSensitivity;
	apply(net, casualSensitivity, seen, ctx);
	if (not ctx.contains_event_dep) {
		// if event dependent sensitivity found do not add other sensitivity
		ctx.extend(casualSensitivity);
	}
}

}
}
