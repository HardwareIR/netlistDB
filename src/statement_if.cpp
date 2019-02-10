#include <statement_if.h>

namespace netlistDB {

IfStatement::IfStatement(Net & condition):
		condition(&condition), ifTrue_specified(false), ifFalse_specified(false) {
	condition.ctx.register_node(*this);
	condition.endpoints.push_back(this);
	// condition can change for this reason we have to use pointer on
	// field where condition signal pointer is stored
	backward.push_back(reinterpret_cast<iNode**>(&this->condition));
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

IfStatement & IfStatement::Elif(Net & cond,
		std::initializer_list<Statement*> statements) {
	assert(ifTrue_specified);
	assert(not ifFalse_specified);

	cond.endpoints.push_back(this);
	elseIf.push_back( { &cond, statements });
	backward.push_back(reinterpret_cast<iNode*>(&elseIf[elseIf.size()-1].first));

	return *this;
}

IfStatement & IfStatement::Else(std::initializer_list<Statement*> statements) {
	assert(not ifFalse_specified);
	ifFalse = statements;
	ifFalse_specified = true;
	return *this;
}

IfStatement & If(Net & condition) {
	return *new IfStatement(condition);
}

}
