#pragma once

#include "../netlist.h"
#include "../statement_if.h"
#include "../statement_assignment.h"
#include <set>
#include <vector>

namespace netlistDB {
namespace query {

/*
 * Structural deep compare the statements for equivalence
 **/
class QueryStructuralCmp {
public:
	// @return true if all statements are same (not necessary the same object) in this order
	static bool statements_are_same(Statement & reference,
			const std::vector<Statement*> & statements);

	// @return true if a has same meaning as b
	static bool is_same(const Statement & a, const Statement & b);
	static bool is_same(const Assignment & a, const Assignment & b);
	static bool is_same(const IfStatement & a, const IfStatement & b);
	static bool is_same_statement_vec(const std::vector<Statement*> & a,
			const std::vector<Statement*> & b);

	// @return true if are same Net objects or are constant nets with same value
	static bool is_same_net(const Net & a, const Net & b);
	static bool is_same_net_vec(const std::vector<Net*> & a,
			const std::vector<Net*> & b);
};

}
}
