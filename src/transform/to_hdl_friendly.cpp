#include <netlistDB/transform/to_hdl_friendly.h>
#include <netlistDB/statement_hwprocess.h>
#include <netlistDB/transform/mark_inter_process_nets.h>
#include <netlistDB/transform/statement_to_hwprocess.h>

using namespace std;

namespace netlistDB {
namespace transform {

bool TransformToHdlFriendly::apply(Netlist & ctx) {
	TransformMarkInterProcessNets mp;
	bool modified = mp.apply(ctx);
	vector<Statement*> stms;
	for (auto n : ctx.nodes) {
		auto s = dynamic_cast<Statement*>(n);
		if (s) {
			stms.push_back(s);
			ctx.unregister_node(*s);
		}
	}
	if (stms.size() == 0)
		return modified;

	modified = true;

	vector<HwProcess*> res;
	for (auto stm : stms)
		TransformStatementToHwProcess::apply( { stm }, res, true);

	TransformStatementToHwProcess::reduce(res);
	for (auto p : res)
		ctx.register_node(*p);

	return modified;
}

}
}
