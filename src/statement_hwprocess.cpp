#include <netlistDB/statement_hwprocess.h>

namespace netlistDB {

HwProcess::HwProcess(const std::string & name,
		std::vector<Statement*> statements,
		utils::OrderedSet<iNode*> sensitivityList,
		utils::OrderedSet<Net*> inputs, utils::OrderedSet<Net*> outputs) :
		name(name), statements(statements), sensitivityList(sensitivityList), inputs(
				inputs), outputs(outputs) {
	rank = 0;
	for (auto s : statements)
		rank += s->rank;
}

void HwProcess::visit_child_stm(const std::function<bool(Statement &)> & fn) {
	for (auto stm: statements) {
		if (fn(*stm))
			return;
	}
}

HwProcess::~HwProcess() {
	visit_child_stm([](Statement & stm){
		delete &stm;
		return false;
	});
}

}
