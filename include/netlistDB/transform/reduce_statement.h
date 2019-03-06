#pragma once

#include <netlistDB/netlist.h>
#include <netlistDB/statement_assignment.h>
#include <netlistDB/statement_if.h>

namespace netlistDB {
namespace transform {

// simplify the statement
class NETLISTDB_PUBLIC TransformReduceStatement {
public:

	/*
	 * Merge statements in list to remove duplicated if-then-else trees
	 *
	 * @param stms input statement vector
	 * @param res the output vector for merged statements
	 * @return rank decrease due merging
	 *
	 * @note rank decrease is sum of ranks of reduced statements
	 * @attention statement list has to me mergable
	 **/
	using stm_it_t = typename std::vector<Statement*>::iterator;
	using elseif_t = IfStatement::elseif_t;

	static size_t merge_statements(const std::vector<Statement*> & stms,
			std::vector<Statement*> & res);

	/*
	 * Merge two vector of statements into one
	 *
	 * :return: vector of merged statements
	 * */
	static std::vector<Statement*> merge_statements_vector(
			std::vector<Statement*> & stmsA, std::vector<Statement*> & stmsB);

	// :attention: statements has to be mergable (to check use _is_mergable method)
	static void merge_other_to_this(IfStatement & a, IfStatement & b);
	static void merge_with_other_stm(Statement & a, Statement & b);

	/*
	 * Try reduce statement and put result in res
	 *
	 * @note if statement is reduced it is also disconnected and deleted deleted
	 *      the statement does not have to exists after this call
	 * @return true if IO of this object changed
	 */
	static bool apply(Statement & stm, std::vector<Statement*> & res);
	static bool apply(IfStatement & stm, std::vector<Statement*> & res);
	static bool apply(Assignment & stm, std::vector<Statement*> & res);

	/*
	 * try reduce the statements between each other in list
	 * */
	static std::pair<size_t, bool> apply(std::vector<Statement*> & statements,
			std::vector<Statement*> & new_statements);
	/*
	 * Merge nested IfContarner form else branch to this IfContainer
	 * as elif and else branches
	 * */
	static void merge_nested_if_from_else(IfStatement & self,
			IfStatement & ifStm);

	/*
	 *  Update signal IO after reduce attempt
	 *
	 *  @param self_reduced if true this object was reduced
	 *  @param io_changed if true IO of this object may changed
	 *      and has to be updated
	 *  @param result_statements list of statements which are result
	 *      of reduce operation on this statement
	 */
	static void on_reduce(Statement & stm, bool self_reduced, bool io_changed,
			std::vector<Statement*> & result_statements);
	/*
	 * After merging statements update IO, sensitivity and context
	 *
	 * @attention rank is not updated
	 * @param self the statement which was merged to
	 * @param other the statement which was merged in to "self"
	 */
	static void on_merge(Statement & self, Statement & other);

	//    Collect inputs/outputs from all child statements
	static void collect_io(Statement & self);
};

}
}
