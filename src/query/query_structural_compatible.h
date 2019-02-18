#pragma once

#include "../netlist.h"
#include "../statement_if.h"
#include "../statement_assignment.h"
#include <set>
#include <vector>

namespace netlistDB {
namespace query {

/*
 * Structural deep compare the statements if they can be merged without colision of code granches
 **/
class QueryStructuralComapatible {
public:

	// dispatch is_mergable call for all types
	static bool is_mergable(const Statement & a, const Statement & b);
	static bool is_mergable(const Assignment & a, const Assignment & b);
	static bool is_mergable(const IfStatement & a, const IfStatement & b);
	// Walk statements and compare if they can be merged into one statement vector
	static bool is_mergable(const std::vector<Statement*> & stmsA,
			const std::vector<Statement*> & stmsB);
	static bool is_mergable(const std::vector<Statement*> & stmsA,
			bool a_specified, const std::vector<Statement*> & stmsB,
			bool b_specified);

	// :return: first statement with rank > 1 or nullptr if iterator empty
	static Statement * get_stm_with_branches(
			std::vector<Statement*>::const_iterator & stm_it,
			const std::vector<Statement*>::const_iterator & stm_it_end);

	static bool if_cond_has_effect(const std::vector<Statement*> & ifTrue,
			const std::vector<Statement*> & ifFalse, bool ifFalse_specified,
			const IfStatement::elseif_t & elIfs);

};

}
}
