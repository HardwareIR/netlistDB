#pragma once

#include <string>
#include <netlistDB/netlist.h>
#include <netlistDB/statement_hwprocess.h>
#include <netlistDB/statement_if.h>

namespace netlistDB {
namespace transform {

class TransformStatementToHwProcess {
public:
	void apply(const std::vector<Statement*> & statements,
			std::vector<HwProcess*> & res, bool try_solve_comb_loops);

	/*
	 * name is generated form the output signal with name specified lowest index
	 * if there is no output returns "proc"
	 * */
	std::string name_for_process(const utils::OrderedSet<Net*> outputs);

	/* Apply enclosure on list of statements
	 * (fill all unused code branches with assignments from value specified by enclosure)
	 *
	 * @param parentStm optional parent statement where this list is some branch
	 * @param current_enclosure list of signals for which this statement list is enclosed
	 * @param statements list of statements
	 * @param do_enclose_for selected signals for which enclosure should be used
	 * @param enclosure enclosure values for signals
	 *
	 * @attention original statements parameter can be modified
	 **/
	void fill_stm_list_with_enclosure(Statement * parentStm,
			std::set<Net*> current_enclosure,
			std::vector<Statement*> statements,
			utils::OrderedSet<Net*> do_enclose_for,
			std::map<Net*, Net*> enclosure);

	void fill_enclosure(Statement & self, std::map<Net*, Net*> enclosure);
	void fill_enclosure(IfStatement & self, std::map<Net*, Net*> enclosure);

	/*
	 * Cut all logic from statements which drives signal sig.
	 *
	 * @param dstSignal signal which drivers should be removed
	 * @param statements vector of statements to filter,
	 * 		will remain only statements which are not driving dstSignal
	 * @param separated output vector of filtered statements,
	 * 			 will contain only statements which are driving the dstSignal
	 *
	 * @return True if all input statements were reduced
	 **/
	bool cut_off_drivers_of(Net * dstSignal,
			std::vector<Statement*> & statements,
			std::vector<Statement*> & separated);
	/*
	 * @return self if statement was not modified or new statement which are driving the sig
	 **/
	Statement * cut_off_drivers_of(Statement & self, Net* sig);
	Statement * cut_off_drivers_of(IfStatement & self, Net* sig);

	/* Clean informations about enclosure for outputs and sensitivity
	 * of this statement
	 */
	void clean_signal_meta(Statement & self);
};

}
}
