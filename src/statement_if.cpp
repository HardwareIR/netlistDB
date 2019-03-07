#include <netlistDB/statement_if.h>

namespace netlistDB {

IfStatement::IfStatement(Net & condition) :
		condition(&condition), ifTrue_specified(false), ifFalse_specified(false) {
	_inputs.push_back(&condition);
	condition.endpoints.push_back(this);
	rank = 0;
}

IfStatement & IfStatement::operator()(Statement* statement) {
	return operator ()(std::vector<Statement*> { statement, });
}
IfStatement & IfStatement::operator()(
		std::initializer_list<Statement*> statements) {
	return operator ()(std::vector<Statement*>(statements));
}
IfStatement & IfStatement::operator()(
		const std::vector<Statement*> & statements) {
	assert(not ifTrue_specified);
	assert(not ifFalse_specified);
	assert(elseIf.size() == 0);
	rank += 1;
	ifTrue_specified = true;
	_register_stements(statements, ifTrue);
	return *this;
}

IfStatement & IfStatement::Elif(Net & cond, Statement* statement) {
	return Elif(cond, { statement });
}

IfStatement & IfStatement::Elif(Net & cond,
		std::initializer_list<Statement*> statements) {
	return Elif(cond, std::vector<Statement*>(statements));
}

IfStatement & IfStatement::Elif(Net & cond,
		const std::vector<Statement*> & statements) {
	assert(ifTrue_specified);
	assert(not ifFalse_specified);
	assert(parent == nullptr);
	rank += 1;
	_inputs.push_back(&cond);
	cond.endpoints.push_back(this);

	elseIf.push_back( { &cond, { } });
	auto & stms = elseIf[elseIf.size() - 1].second;

	_register_stements(statements, stms);

	return *this;
}

IfStatement & IfStatement::Else(Statement* statement) {
	return Else(std::vector<Statement*> { statement });
}

IfStatement & IfStatement::Else(std::initializer_list<Statement*> statements) {
	return Else(std::vector<Statement*>(statements));
}

IfStatement & IfStatement::Else(const std::vector<Statement*> & statements) {
	assert(not ifFalse_specified);
	assert(parent == nullptr);
	ifFalse_specified = true;
	rank += 1;
	_register_stements(statements, ifFalse);

	return *this;
}

void IfStatement::visit_child_stm(const std::function<bool(Statement &)> & fn) {
	for (auto stm : ifTrue) {
		if (fn(*stm))
			return;
	}

	for (auto & eif : elseIf) {
		for (auto stm : eif.second) {
			if (fn(*stm))
				return;
		}
	}

	if (ifFalse_specified) {
		for (auto stm : ifFalse) {
			if (fn(*stm))
				return;
		}
	}
}

IfStatement::~IfStatement() {
	visit_child_stm([](Statement & stm) {
		delete &stm;
		return false;
	});
}

NETLISTDB_PUBLIC IfStatement & If(Net & condition) {
	auto i = new IfStatement(condition);
	condition.ctx.register_node(*i);
	return *i;
}

}
