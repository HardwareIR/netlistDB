#include "statement_if.h"

namespace netlistDB {

IfStatement::IfStatement(Net & condition) :
		condition(&condition), ifTrue_specified(false), ifFalse_specified(false) {

	_inputs.push_back(&condition);
	condition.endpoints.push_back(this);
	condition.ctx.register_node(*this);
}

IfStatement & IfStatement::operator()(
		std::initializer_list<Statement*> statements) {
	assert(not ifTrue_specified);
	assert(not ifFalse_specified);
	assert(elseIf.size() == 0);

	ifTrue_specified = true;
	_register_stements(statements, ifTrue);
	return *this;
}

IfStatement & IfStatement::Elif(Net & cond,
		std::initializer_list<Statement*> statements) {
	assert(ifTrue_specified);
	assert(not ifFalse_specified);
	assert(parent == nullptr);
	rank += 1;
	_inputs.push_back(&cond);
	cond.endpoints.push_back(this);

	elseIf.push_back( { &cond, {} });
	auto & stms = elseIf[elseIf.size() - 1].second;

	_register_stements(statements, stms);

	return *this;
}

IfStatement & IfStatement::Else(std::initializer_list<Statement*> statements) {
	assert(not ifFalse_specified);
	assert(parent == nullptr);
	ifFalse = statements;
	ifFalse_specified = true;
	rank += 1;
	_register_stements(statements, ifFalse);

	return *this;
}

utils::ChainedSequence<Statement*> IfStatement::_iter_stms() {
	utils::ChainedSequence<Statement*> it;
	it.push_back(&ifTrue);
	for (auto eif : elseIf) {
		it.push_back(&eif.second);
	}
	if (ifFalse_specified) {
		it.push_back(&ifFalse);
	}

	return it;
}

IfStatement & If(Net & condition) {
	return *new IfStatement(condition);
}

}
