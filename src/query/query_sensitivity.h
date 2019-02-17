#include "../netlist.h"
#include "../statement_assignment.h"
#include "../statement_if.h"

namespace netlistDB {
namespace query {

class QuerySensitivity {
public:
	/*
	 * Discover the sensitivity of all statements
	 * */
	static void apply(Netlist & ctx);
	static void apply(Statement & stm, std::set<iNode*> & seen);
	static void apply(Assignment & assig, std::set<iNode*> & seen);
	static void apply(IfStatement & ifstm, std::set<iNode*> & seen);

};

}
}
