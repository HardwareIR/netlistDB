#pragma once

#include <netlistDB/netlist.h>

namespace netlistDB {

class NETLISTDB_PUBLIC _IfSttementSensExtra {
public:
	std::set<Net*> ifTrue_enclosed_for;
	std::vector<std::set<Net*>> elseIf_enclosed_for;
	std::set<Net*> ifFalse_enclosed_for;
};

/*
 * The representation if the if statement for the netlist
 *
 * The if statement can represent multiple hardware components like multiplexer or latch
 * */
class NETLISTDB_PUBLIC IfStatement: public Statement {
public:
	Net * condition;

	bool ifTrue_specified;
	std::vector<Statement *> ifTrue;

	using elseif_t = std::vector<std::pair<Net *, std::vector<Statement *> > >;
	elseif_t elseIf;

	bool ifFalse_specified;
	std::vector<Statement *> ifFalse;

	_IfSttementSensExtra sens_extra;

	/*
	 * Condition of the if statement
	 * */
	IfStatement(Net & condition);
	// the if true branch
	IfStatement & operator()(Statement* statement);
	IfStatement & operator()(std::initializer_list<Statement*> statements);
	IfStatement & operator()(const std::vector<Statement*> & statements);
	// elif branch
	IfStatement & Elif(Net & cond, Statement* statement);
	IfStatement & Elif(Net & cond,
			std::initializer_list<Statement*> statements);
	IfStatement & Elif(Net & cond, const std::vector<Statement*> & statements);
	// else branch
	IfStatement & Else(Statement* statement);
	IfStatement & Else(std::initializer_list<Statement*> statements);
	IfStatement & Else(const std::vector<Statement*> & statements);

	virtual void visit_child_stm(const std::function<bool(Statement &)> & fn) override;
	virtual ~IfStatement();
};

// the helper method for IfStatement construction
NETLISTDB_PUBLIC IfStatement & If(Net & condition);

}
