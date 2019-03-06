#pragma once

#include <set>
#include <vector>

#include <netlistDB/netlist.h>
#include <netlistDB/statement_if.h>
#include <netlistDB/statement_assignment.h>
#include <netlistDB/statement_hwprocess.h>

namespace netlistDB {
namespace query {

/*
 * Visit all objects in the statement and expressions which are part of this statement
 *
 * @note the boundary nets has to have .id.hidden flag set to false
 * */
class NETLISTDB_PUBLIC StatementVisitor {
public:
	virtual void visit(const Assignment & a);
	virtual void visit(const IfStatement & i);
	virtual void visit(const std::vector<Statement*> stms);
	virtual void visit(const HwProcess & p);
	virtual void visit(const Statement & stm);
	virtual void visit(const FunctionCall & c);
	/*
	 * Visit all items in expression behind the net (only if this net is hidden)
	 * */
	virtual void visit(const Net & n);
	virtual void visit(const OperationNode & op);

	virtual ~StatementVisitor();
};

}
}
