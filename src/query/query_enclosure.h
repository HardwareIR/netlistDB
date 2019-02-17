#include "../netlist.h"
#include "../statement_if.h"
#include "../statement_assignment.h"
#include <set>
#include <vector>

namespace netlistDB {
namespace query {


/*
 * Discover for which signals the statement is not latch
 * The result is stored in sens property of the statement itself
 * */
class QueryEnclosure {
	// dispatch apply call for all statement types
	static void apply(Statement & stm);
	static void apply(Assignment & a);
	/*
	 * Discover the enclosure for if statement and all potential child statements
	 * */
	static void apply(IfStatement & ifstm);
	/*
	 * Discover enclosure for vector of statements in same code branch
	 *
	 * @param statements statements in one code branch
	 * @param outputs outputs which should be driven from this statements
	 * @return set of signals for which this statements have always some drive
	 *     (is enclosed)
	 * */
	static std::set<Net*> apply(const std::vector<Statement*> & statements,
			const utils::OrderedSet<Net*> & outputs);
};

}
}
