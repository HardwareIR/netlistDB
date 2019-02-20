#pragma once

#include <string>
#include <netlistDB/netlist.h>
#include <netlistDB/statement_hwprocess.h>
#include <netlistDB/statement_if.h>

namespace netlistDB {
namespace transform {

/*
 * The process is a wrapper around the statement list.
 * It is possible just wrap each statement in to separate process, but this would
 * result in to unreadable code. (And this transformation is meant to be used for the serialization of circuit
 * in to readable format (Verilog etc.))
 *
 * The main source of the bloat and duplicated code are similar statements which have same branching structure
 * but are using different signals. For example there can be multiple processes
 * which are describing the RTL register, all this processes can be merged all together.
 * However the combinational loops may appear and the process can not contain combinational loop inside.
 *
 *  1. Split statement in to groups which have same branching structure.
 *  2. If group have combinational loop try to cut off the driver of the signal out.
 *  3. Merge all statements with similar structure to a single statement.
 *  4. Wrap result in to HwProcess instance.
 *
 * */

class TransformStatementToHwProcess {
public:

	// Statements are not comparable due incompatible structure
	class IncompatibleStructure: public std::runtime_error {
	public:
		IncompatibleStructure(): std::runtime_error("") {}
	};

	/*
	 * @attention the statements does not have to stay same in circuit
	 *    (they can be merged with something else or removed).
	 * */
	static void apply(const std::vector<Statement*> & statements,
			std::vector<HwProcess*> & res, bool try_solve_comb_loops);

	/*
	 * name is generated form the output signal with name specified lowest index
	 * if there is no output returns "proc"
	 * */
	static std::string name_for_process(const utils::OrderedSet<Net*> outputs);

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
	static void fill_stm_list_with_enclosure(Statement * parentStm,
			std::set<Net*> current_enclosure,
			std::vector<Statement*> statements,
			utils::OrderedSet<Net*> do_enclose_for,
			std::map<Net*, Net*> enclosure);

	static void fill_enclosure(Statement & self,
			std::map<Net*, Net*> enclosure);
	static void fill_enclosure(IfStatement & self,
			std::map<Net*, Net*> enclosure);

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
	static bool cut_off_drivers_of(Net * dstSignal,
			std::vector<Statement*> & statements,
			std::vector<Statement*> & separated);
	/*
	 * @return self if statement was not modified or new statement which are driving the sig
	 **/
	static Statement * cut_off_drivers_of(Statement & self, Net* sig);
	static Statement * cut_off_drivers_of(IfStatement & self, Net* sig);

	/* Clean informations about enclosure for outputs and sensitivity
	 * of this statement.
	 */
	static void clean_signal_meta(Statement & self);

	// Get maximum _instId from all assignments in statement
	static size_t max_stm_id(Statement & stm);

	/* get max statement id,
	 * used for sorting of processes in architecture
	 */
	static size_t max_stm_id(HwProcess & proc);

	// Try to merge processes as much is possible
	static void reduce_processes(std::vector<HwProcess*> & processes);

	/* check if process is just unconditional assignments
	 * and it is useless to merge them
	 */
	static bool checkIfIsTooSimple(HwProcess & proc);

	/* Try merge procB into procA
	 *
	 * @raise IncompatibleStructure if merge is not possible
	 * @attention procA is now result if merge has succeed
	 * @return procA which is now result of merge
	 */
	static HwProcess * tryToMerge(HwProcess & procA, HwProcess & procB);
};

}
}
