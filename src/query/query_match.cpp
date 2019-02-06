#include <algorithm>
#include <iostream>

#include "query_match.h"
#include "statement_if.h"

using namespace std;

namespace netlistDB {
namespace query {

QueryMatch::QueryMatch() :
		Netlist("") {
}

std::vector<QueryMatch::match_t> QueryMatch::search(Netlist & netlist) {
	std::vector<match_t> matches;

	// find first defined signal
	Net * root_sig = nullptr;
	for (auto n: nets) {
		root_sig = n;
		if (root_sig)
			break;
	}
	if (root_sig == nullptr) {
		// empty match
		return matches;
	}

	// it is expected that query size is much smaller than graph itself
	for (auto net : netlist.nets) {
		if (net == nullptr)
			continue;

		// query graph has only single component
		// if the signal matches whole matching graph is discovered
		// because match is performed in all directions
		match_t current_match;
		BackTrackingContext ctx(current_match);
		if (search_recurse(*root_sig, *net, ctx)) {
			matches.push_back(current_match);
		}
		ctx.discard();
		current_match.clear();
	}
	return matches;
}

bool QueryMatch::find_matching_permutation(OrderedSet<OperationNode*> & ref,
		OrderedSet<OperationNode*> & graphIo, BackTrackingContext& ctx,
		bool allow_more_in_graph) {

	if (allow_more_in_graph) {
		if (graphIo.size() < ref.size())
			return false;
	} else {
		if (ref.size() != graphIo.size())
			return false;
	}

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

bool QueryMatch::search_recurse(Net & ref, Net & net,
		BackTrackingContext& ctx) {
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
	// as majority of the nets has only one driver)

	// ignore drivers and the net can have more endpoints
	bool ignore_drivers = ref.direction == Direction::DIR_IN;

	bool ignore_endpoints = ref.direction == Direction::DIR_OUT;
	if (ignore_drivers and ignore_endpoints) {
		// matching any signal
		return true;
	}

	auto & child_ctx = ctx.child();

	if (ignore_drivers) {
		// matching the input signal which can potentially have more endpoints,
		// which we does not have to match

		return find_matching_permutation(ref.endpoints, net.endpoints,
				child_ctx, true);
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
		if (not find_matching_permutation(ref.drivers, net.drivers, child_ctx,
				false)) {
			return false;
		}

		// the number of endpoints can be potentially large number
		// we have to check all combinations of input an outputs
		// both direction has to be checked as the circuit can contain the cycle
		return ignore_endpoints
				or find_matching_permutation(ref.endpoints, net.endpoints,
						child_ctx, false);
	}
}

bool QueryMatch::search_recurse(iNode & ref, iNode & n,
		BackTrackingContext & ctx) {
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
	for (auto refElif : ref.elseIf) {
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
