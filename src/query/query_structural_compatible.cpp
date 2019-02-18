#include <netlistDB/query/query_structural_compatible.h>
#include <netlistDB/query/query_structural_cmp.h>

using namespace std;
namespace netlistDB {
namespace query {

bool QueryStructuralComapatible::is_mergable(const Statement & a,
		const Statement & b) {
	auto * aa = dynamic_cast<const Assignment*>(&a);
	auto * ab = dynamic_cast<const Assignment*>(&b);
	if (aa and ab)
		return is_mergable(*aa, *ab);
	auto * ia = dynamic_cast<const IfStatement*>(&a);
	auto * ib = dynamic_cast<const IfStatement*>(&b);
	if (ia and ib)
		return is_mergable(*ia, *ib);
	return false;
}

bool QueryStructuralComapatible::is_mergable(const Assignment & a,
		const Assignment & b) {
	return true;
}

bool QueryStructuralComapatible::is_mergable(const IfStatement & a,
		const IfStatement & b) {
	if (a.condition != b.condition or not is_mergable(a.ifTrue, a.ifTrue))
		return false;

	if (a.elseIf.size() != b.elseIf.size()) {
		return false;
	}

	auto b_it = b.elseIf.begin();
	for (auto ait : a.elseIf) {
		if (ait.first != b_it->first
				or not is_mergable(ait.second, b_it->second))
			return false;
		b_it++;
	}
	if (not is_mergable(a.ifFalse, a.ifFalse_specified, b.ifFalse,
			b.ifFalse_specified)) {
		return false;
	}
	return true;
}

bool QueryStructuralComapatible::is_mergable(
		const std::vector<Statement*> & stmsA, bool a_specified,
		const std::vector<Statement*> & stmsB, bool b_specified) {
	if (not a_specified and not b_specified)
		return true;
	else if (not a_specified or not b_specified)
		return false;
	return is_mergable(stmsA, stmsB);
}

Statement * QueryStructuralComapatible::get_stm_with_branches(
		vector<Statement*>::const_iterator & stm_it,
		const vector<Statement*>::const_iterator & stm_it_end) {
	while ((stm_it != stm_it_end)) {
		if ((*stm_it)->rank > 0) {
			return *stm_it;
		}
		stm_it++;
	}
	return nullptr;
}

bool QueryStructuralComapatible::is_mergable(const vector<Statement*> & stmsA,
		const vector<Statement*> & stmsB) {
	auto a_it = stmsA.begin();
	auto a_end = stmsA.end();
	auto b_it = stmsB.begin();
	auto b_end = stmsB.end();

	Statement * a = get_stm_with_branches(a_it, a_end);
	Statement * b = get_stm_with_branches(b_it, b_end);
	while (a != nullptr or b != nullptr) {
		if (a == nullptr or b == nullptr or not is_mergable(*a, *b)) {
			return false;
		}

		a = get_stm_with_branches(a_it, a_end);
		b = get_stm_with_branches(b_it, b_end);
	}
	// lists are empty
	return true;

}

bool QueryStructuralComapatible::if_cond_has_effect(
		const vector<Statement*> & ifTrue, const vector<Statement*> & ifFalse,
		bool ifFalse_specified, const IfStatement::elseif_t & elIfs) {
	size_t stmCnt = ifTrue.size();
	// [TODO] condition in empty if stm
	if (not ifFalse_specified)
		return true; // is a latch
	if (stmCnt != ifFalse.size())
		return true; // different contend in branches

	// collect all statement vector and check it's size
	vector<const vector<Statement*>*> stms_vectors;
	stms_vectors.reserve(elIfs.size() + 1);
	for (auto &c : elIfs) {
		if (c.second.size() != stmCnt)
			return true;
		stms_vectors.push_back(&c.second);
	}
	stms_vectors.push_back(&ifFalse);

	// test if all statement in all statement vectors are same
	size_t i = 0;
	vector<Statement*> stm_tmp;
	for (auto s : ifTrue) {
		for (auto s_v : stms_vectors) {
			stm_tmp.push_back((*s_v)[i]);
		}
		if (not query::QueryStructuralCmp::statements_are_same(*s, stm_tmp))
			return true;
		i++;
		stm_tmp.clear();
	}
	return false;
}

}
}
