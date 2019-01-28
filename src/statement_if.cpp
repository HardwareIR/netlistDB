#include "statemen_if.h"

namespace netlistDB {

IfStatement::IfStatement(Signal & condition) :
		condition(condition), ifTrue_specified(false), ifFalse_specified(false) {
}

IfStatement & IfStatement::operator()(
		std::initializer_list<Statement*> statements) {
	assert(not ifTrue_specified);
	assert(not ifFalse_specified);
	assert(elseIf.size() == 0);

	ifTrue = statements;
	ifTrue_specified = true;
	return *this;
}

IfStatement & IfStatement::Elif(Signal & cond,
		std::initializer_list<Statement*> statements) {
	assert(not ifFalse_specified);
	ifTrue_specified = true;
	return *this;
}

IfStatement & IfStatement::Else(std::initializer_list<Statement*> statements) {
	assert(not ifFalse_specified);
	ifFalse = statements;
	ifFalse_specified = true;
	return *this;
}

IfStatement & If(Signal & condition) {
	return * new IfStatement(condition);
}

}
