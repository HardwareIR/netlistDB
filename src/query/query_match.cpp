#include <algorithm>
#include <iostream>

#include "query_match.h"
#include "statemen_if.h"

using namespace std;

// http://www.scielo.br/scielo.php?script=sci_arttext&pid=S0101-74382005000300005
// https://stackoverflow.com/questions/8176298/vf2-algorithm-steps-with-example
// https://stackoverflow.com/questions/17480142/is-there-any-simple-example-to-explain-ullmann-algorithm
// https://stackoverflow.com/questions/13537716/how-to-partially-compare-two-graphs/13537776#13537776
// https://github.com/charlieegan3/graph_match
// https://github.com/chaihf/BFS-OpenMP
// https://www.cs.cmu.edu/~scandal/nesl/algorithms.html#mis
// https://github.com/gbossi/Pagerank-DOBFS
// https://github.com/narayanan2004/GraphMat
// https://github.com/alex-87/HyperGraphLib
// https://www.sanfoundry.com/cpp-programming-examples-graph-problems-algorithms/
// https://github.com/cpp-taskflow/cpp-taskflow
// https://stackoverflow.com/questions/44082755/parallelizing-a-breadth-first-search
// http://supertech.csail.mit.edu/papers/pbfs.pdf
// http://crpit.com/confpapers/CRPITV127Dinneen.pdf

namespace netlistDB {
namespace query {

bool QueryMatch::search_recurse(FunctionCall & ref, FunctionCall & call,
		BackTrackingContext& ctx) {
	switch (ctx.check_can_match(ref, call)) {
	case BackTrackingContext::already_matches:
		return true;
	case BackTrackingContext::can_match:
		break;
	case BackTrackingContext::can_not_match:
		return false;
	}

	// check all io if matches
	if (ref.args.size() != call.args.size())
		return false;

	ctx.insert_match(ref, call);

	// check if result signal matches
	if (not search_recurse(ref.res, call.res, ctx)) {
		return false;
	}

	// check if all inputs matches
	auto _ref = ref.args.begin();
	for (auto a : call.args) {
		Net & __ref = **_ref;
		if (not search_recurse(__ref, *a, ctx)) {
			return false;
		}
		++_ref;
	}
	return true;
}

QueryMatch::QueryMatch() :
		Netlist(""), query_size(0) {
}

std::vector<QueryMatch::match_t> QueryMatch::search(Netlist & netlist) {
	std::vector<match_t> matches;
	Net * root_sig = nullptr;
	for (auto ref : nets) {
		if (root_sig == nullptr or root_sig->index > ref->index)
			root_sig = ref;
	}
	if (root_sig == nullptr) {
		// empty query -> no match
		return matches;
	}

	// it is expected that query size is much smaller than graph itself
	for (auto net : netlist.nets) {
		// query graph has only single component
		// if the signal matches whole matching graph is discovered

		match_t current_match;
		BackTrackingContext ctx(current_match);
		if (search_recurse(*root_sig, *net, ctx)) {
			matches.push_back(current_match);
		} else {
		}
		ctx.discard();
		current_match.clear();
	}
	return matches;
}
/**
 * Find first matching combination
 * \note The match is stored in ctx.match and can be canceled by methods of ctx
 * \param ref set of operations from query
 * \param graphIo set of operations from the queried graph
 * \return true if match was found
 **/
bool QueryMatch::find_matching_permutation(OrderedSet<OperationNode*> & ref,
		OrderedSet<OperationNode*> & graphIo, BackTrackingContext& ctx) {
	if (ref.size() != graphIo.size())
		return false;

	bool match_found;
	// we can not mask used items in "permutation" as they may appear
	// appear many times, but query will end immediately if the item is used
	for (auto _ref : ref) {
		match_found = false;
		auto & ctx_for_endpoint = ctx.child();
		for (auto _graph : graphIo) {
			if (not search_recurse(*_ref, *_graph, ctx_for_endpoint)) {
				ctx_for_endpoint.discard();
			} else {
				match_found = true;
				break;
			}
		}
		if (not match_found) {
			ctx.pop_child();
			// on e reference from query graph does not match with the graph
			// the graph can not match the query
			break;
		}
	}

	return match_found;
}

/**
 * Signal matches if each of its connected nodes matches in any order
 * Node matches if it has same type and structure and all of its connected
 * signals matches in same order
 *
 * \param ref the net from the query
 * \param net the net from the queried graph
 * \return true if match was found
 **/
bool QueryMatch::search_recurse(Net & ref, Net & net, BackTrackingContext& ctx) {
	switch (ctx.check_can_match(ref, net)) {
	case BackTrackingContext::already_matches:
		// this signal already matched this reference somewhere else
		return true;
	case BackTrackingContext::can_match:
		// this signal can match the reference as it was not used before
		break;
	case BackTrackingContext::can_not_match:
		// this reference is matched to different signal
		return false;
	}
	ctx.insert_match(ref, net);

	// order of endpoint or drivers does not matter (as signal is hyperedge)
	// test all combinations of paths (but with very high probability only single item
	//  as majority of the nets has only one driver)

	bool ignore_dirvers = ref.drivers.size() == 0;
	bool ignore_endpoints = ref.endpoints.size() == 0;
	if (ignore_dirvers and ignore_endpoints) {
		// matching any signal
		return true;
	}

	auto & child_ctx = ctx.child();

	if (ignore_dirvers) {
		// matching the input signal which can potentially have more endpoints,
		// which we does not have to match

		return find_matching_permutation(ref.endpoints, net.endpoints,
				child_ctx);
	} else {
		// check if any permutation of drivers and endpoints matches
		// we need to check if all IO matches
		if (ref.drivers.size() != net.drivers.size()
				or ref.endpoints.size() != net.endpoints.size())
			return false;

		// [clue]: the drivers and endpoints can be separately as they can collide only it there
		// is a undirected cycle in graph, however if there is such a cycle we have to discovery
		// it in first run and the check of endpoints have to use it

		// vast majority of nets has only one driver
		if (not find_matching_permutation(ref.drivers, net.drivers,
				child_ctx)) {
			return false;
		}

		// the number of endpoints can be potentially large number
		// we have to check all combinations of input an outputs
		// both direction has to be checked as the circuit can contain the cycle
		return ignore_endpoints
				or find_matching_permutation(ref.endpoints, net.endpoints,
						child_ctx);
	}
}

bool QueryMatch::search_recurse(iNode & ref, iNode & n, BackTrackingContext & ctx) {
	auto fA = dynamic_cast<FunctionCall*>(&ref);
	auto fB = dynamic_cast<FunctionCall*>(&n);
	if (fA and fB) {
		return search_recurse(*fA, *fB, ctx);
	} else if (fA or fB) {
		return false;
	}

	auto nA = dynamic_cast<Net*>(&ref);
	auto nB = dynamic_cast<Net*>(&n);
	if (nA and nB) {
		return search_recurse(*nA, *nB, ctx);
	} else if (nA or nB) {
		return false;
	}

	auto aA = dynamic_cast<Assignment*>(&ref);
	auto aB = dynamic_cast<Assignment*>(&n);
	if (aA and aB) {
		return search_recurse(*aA, *aB, ctx);
	}

	auto ifA = dynamic_cast<IfStatement*>(&ref);
	auto ifB = dynamic_cast<IfStatement*>(&n);
	if (ifA and ifB) {
		return search_recurse(*ifA, *ifB, ctx);
	}

	return false;
}

bool QueryMatch::statements_matches(std::vector<Statement *> & ref,
		std::vector<Statement *> & n, BackTrackingContext & ctx) {
	auto tStm = n.begin();
	for (auto refTStm : ref) {
		if (not search_recurse(*refTStm, **tStm, ctx))
			return false;
		++tStm;
	}
	return true;
}

bool QueryMatch::search_recurse(IfStatement & ref, IfStatement & n,
		BackTrackingContext & ctx) {
	if (ref.elseIf.size() != n.elseIf.size())
		return false;

	if (ref.ifTrue_specified != n.ifTrue_specified
			or ref.ifFalse_specified != n.ifFalse_specified)
		return false;

	if (not search_recurse(ref.condition, n.condition, ctx))
		return false;

	if (not statements_matches(ref.ifTrue, n.ifFalse, ctx))
		return false;

	auto elif = n.elseIf.begin();
	for (auto refElif: ref.elseIf) {
		if (not search_recurse(*refElif.first, *elif->first, ctx))
			return false;

		vector<Statement*> & r = refElif.second;
		vector<Statement*> & g = elif->second;

		if (not statements_matches(r, g, ctx))
			return false;

		++elif;
	}

	return true;
}

bool QueryMatch::search_recurse(Assignment & ref, Assignment & n,
		BackTrackingContext & ctx) {
	if (ref.dst_index.size() != n.dst_index.size())
		return false;

	if (not search_recurse(ref.dst, n.dst, ctx))
		return false;

	auto it = n.dst_index.begin();
	for (auto refi : ref.dst_index) {
		if (not search_recurse(*refi, **it, ctx))
			return false;
		++it;
	}

	if (not search_recurse(ref.src, n.src, ctx))
		return false;

	return true;
}


}
}
