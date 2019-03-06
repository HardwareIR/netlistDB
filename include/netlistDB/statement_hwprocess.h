#pragma once

#include <string>

#include <netlistDB/netlist.h>
#include <netlistDB/utils/ordered_set.h>

namespace netlistDB {

/*
 * Hdl process - the container for statements in HDL
 *
 * @ivar name name used as id in target HDL
 * @ivar statements vector of statements in body of process
 * @ivar sensitivity_list set of Net or event operators which
 *     are describing when should this process be re-evaluated
 *
 * @ivar _inputs all input signals for this process
 * @ivar _outputs all output signals for this process
 * @ivar count of branches in statements, used as hint for process complexity
 *
 * @note HwProcess do not have to be process in target HDL, for example
 *     simple process which contains only unconditional assignment will
 *     be rendered just as assignment
 */
class NETLISTDB_PUBLIC HwProcess: public Statement {
public:
	std::string name;
	std::vector<Statement*> statements;
	utils::OrderedSet<iNode*> sensitivityList;
	utils::OrderedSet<Net*> inputs;
	utils::OrderedSet<Net*> outputs;

	HwProcess(const std::string & name, std::vector<Statement*> statements,
			utils::OrderedSet<iNode*> sensitivityList,
			utils::OrderedSet<Net*> inputs, utils::OrderedSet<Net*> outputs);

	virtual utils::ChainedSequence<Statement*> _iter_stms() override;

};

}
