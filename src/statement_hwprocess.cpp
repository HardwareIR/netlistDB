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

utils::ChainedSequence<Statement*> HwProcess::_iter_stms() {
	utils::ChainedSequence<Statement*> it;
	it.push_back(&statements);
	return it;
}

HwProcess::~HwProcess() {
	for (auto stm : _iter_stms()) {
		delete stm;
	}
}

}
