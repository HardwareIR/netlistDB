#pragma once

#include "netlist.h"

namespace netlistDB {


class IfStatement: public Statement {
public:
	Net & condition;

	bool ifTrue_specified;
	std::vector<Statement *> ifTrue;

	std::vector<std::pair<Net *, Statement *>> elseIf;

	bool ifFalse_specified;
	std::vector<Statement *> ifFalse;

	IfStatement(Net & condition);
	IfStatement & operator()(std::initializer_list<Statement*> statements);
	IfStatement & Elif(Net & cond, std::initializer_list<Statement*> statements);
	IfStatement & Else(std::initializer_list<Statement*> statements);

};
IfStatement & If(Net & condition);


}
