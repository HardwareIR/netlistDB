#include "../netlist.h"
#include "../statement_assignment.h"
#include "../statement_if.h"

namespace netlistDB {
namespace transform {

// simplify the statement
class TransformReduceStatement {
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
	size_t merge_statements(const std::vector<Statement*> & stms, std::vector<Statement*> & res);

	/*
	 * try reduce statement and put result in res
	 *
	 * @note if statement is reduced it is also disconnected and deleted deleted
	 *      the statement does not have to exists after this call
	 * @return true if IO of this object changed
	 */
	bool apply(Statement & stm, std::vector<Statement*> & res);
	bool apply(IfStatement & stm, std::vector<Statement*> & res);
	bool apply(Assignment & stm, std::vector<Statement*> & res);
};

}
}
