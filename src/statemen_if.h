#pragma once

#include "netlist.h"

namespace netlistDB {


class IfStatement: public Statement {
public:
	Signal & condition;

	bool ifTrue_specified;
	std::vector<Statement *> ifTrue;

	std::vector<std::pair<Signal *, Statement *>> elseIf;

	bool ifFalse_specified;
	std::vector<Statement *> ifFalse;

	IfStatement(Signal & condition);
	IfStatement & operator()(std::initializer_list<Statement*> statements);
	IfStatement & Elif(Signal & cond, std::initializer_list<Statement*> statements);
	IfStatement & Else(std::initializer_list<Statement*> statements);

};
IfStatement & If(Signal & condition);


}
