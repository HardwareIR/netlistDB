#include "query_structural_cmp.h"

using namespace std;

namespace netlistDB {
namespace query {

bool QueryStructuralCmp::is_same(const Statement & a, const Statement & b) {
	auto aa = dynamic_cast<const Assignment*>(&a);
	auto ab = dynamic_cast<const Assignment*>(&b);
	if (aa and ab)
		return is_same(*aa, *ab);
	auto * ia = dynamic_cast<const IfStatement*>(&a);
	auto * ib = dynamic_cast<const IfStatement*>(&b);
	if (ia and ib)
		return is_same(*ia, *ib);
	return false;
}
bool QueryStructuralCmp::is_same(const Assignment & a, const Assignment & b) {
	return (is_same_net(a.dst, b.dst) and is_same_net(a.src, b.src)
			and is_same_net_vec(a.dst_index, b.dst_index));
}

bool QueryStructuralCmp::is_same(const IfStatement & a, const IfStatement & b) {
	if (&a == &b)
		return true;

	if (a.rank != b.rank)
		return false;

	if (is_same_net(*a.condition, *b.condition)) {
		if (a.ifTrue.size() == b.ifTrue.size()
				and a.ifFalse_specified == b.ifFalse_specified
				and a.ifFalse.size() == b.ifFalse.size()
				and a.elseIf.size() == b.elseIf.size()) {
			if (not is_same_statement_vec(a.ifTrue, b.ifTrue)
					or not is_same_statement_vec(a.ifFalse, b.ifFalse))
				return false;
			for (size_t i = 0; i < a.elseIf.size(); i++) {
				auto & ac = a.elseIf[i];
				auto & bc = b.elseIf[i];
				if (not (is_same_net(*ac.first, *bc.first)
						or not is_same_statement_vec(ac.second, bc.second))) {
					return false;
				}
			}
			return true;
		}
	}
	return false;

}

bool QueryStructuralCmp::is_same_statement_vec(
		const vector<Statement*> & stmListA,
		const vector<Statement*> & stmListB) {
	if (&stmListA == &stmListB) {
		return true;
	}

	for (size_t i = 0; i < stmListA.size(); i++) {
		if (not is_same(*stmListA[i], *stmListB[i]))
			return false;
	}
	return true;
}

bool QueryStructuralCmp::statements_are_same(Statement & reference,
		const vector<Statement*> & statements) {
	for (auto s : statements)
		if (not is_same(reference, *s))
			return false;
	return true;
}

bool QueryStructuralCmp::is_same_net(const Net & a, const Net & b) {
	return (&a == &b
			or (a.val != nullptr and b.val != nullptr and *a.val == *b.val));
}

bool QueryStructuralCmp::is_same_net_vec(const std::vector<Net*> & nets_a,
		const std::vector<Net*> & nets_b) {
	if (nets_a.size() != nets_a.size())
		return false;

	for (size_t i = 0; i < nets_a.size(); i++) {
		if (not is_same_net(*nets_a[i], *nets_b[i]))
			return false;
	}
	return true;
}



}
}
