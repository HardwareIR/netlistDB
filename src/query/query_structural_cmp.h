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
	//  :return: True if all statements have same branching structure same
	static bool statements_are_same(Statement & reference,
			const std::vector<Statement*> & statements);

	// :return: True if a has same meaning as b
	static bool is_same(const Statement & a, const Statement & b);
	static bool is_same(const Assignment & a, const Assignment & b);
	static bool is_same(const IfStatement & a, const IfStatement & b);
	static bool is_same_statement_vec(const std::vector<Statement*> & a,
			const std::vector<Statement*> & b);

	static bool is_same_net(const Net & a, const Net & b);
	static bool is_same_net_vec(const std::vector<Net*> & a,
			const std::vector<Net*> & b);
};

}
}
