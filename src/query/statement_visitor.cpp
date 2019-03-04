#include <netlistDB/query/statement_visitor.h>

namespace netlistDB {
namespace query {

void StatementVisitor::visit(const Assignment & a) {
	visit(a.src);
	for (auto i : a.dst_index) {
		visit(*i);
	}
	assert(a.dst.id.hidden == false);
	visit(a.dst);
}
void StatementVisitor::visit(const IfStatement & i) {
	visit(*i.condition);
	visit(i.ifTrue);
	for (auto & c : i.elseIf) {
		visit(*c.first);
		visit(c.second);
	}
	visit(i.ifFalse);
}
void StatementVisitor::visit(const std::vector<Statement*> stms) {
	for (auto s : stms) {
		visit(*s);
	}
}
void StatementVisitor::visit(const HwProcess & p) {
	visit(p.statements);
}
void StatementVisitor::visit(const Statement & stm) {
	auto a = dynamic_cast<const Assignment*>(&stm);
	if (a) {
		visit(*a);
		return;
	}
	auto i = dynamic_cast<const IfStatement *>(&stm);
	if (i) {
		visit(*i);
		return;
	}
	throw std::runtime_error(
			std::string(__PRETTY_FUNCTION__) + " unknown statement type");
}
void StatementVisitor::visit(const FunctionCall & c) {
	for (auto n : c.args) {
		visit(*n);
	}
}
/*
 * Visit all items in expression behind the net (only if this net is hidden)
 * */
void StatementVisitor::visit(const Net & n) {
	if (n.id.hidden and n.val == nullptr) {
		for (auto d : n.drivers) {
			visit(*d);
		}
	}
}

void StatementVisitor::visit(const OperationNode & op) {
	auto o = dynamic_cast<const FunctionCall*>(&op);
	if (o == nullptr) {
		throw std::runtime_error(
				std::string(__PRETTY_FUNCTION__)
						+ " expected only FunctionCall as this function should walk only items in expression");
	}
	visit(*o);
}

StatementVisitor::~StatementVisitor() {
}

}
}
