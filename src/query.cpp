#include "query.h"

#include <algorithm>
#include <iostream>

namespace netlistDB {
using namespace std;

Query::Query() :
		Netlist(""), query_size(0) {
}

std::vector<Query::match_t> Query::search(Netlist & netlist) {
	size_t sig_cnt = signals.size();
	// input signals vector is expected to be much smaller
	// as it represents only this query
	std::vector<match_t> matches;

	vector<Net*> inputs(sig_cnt);
	inputs.resize(sig_cnt);
	fill(inputs.begin(), inputs.end(), nullptr);
	for (auto & net : signals) {
		inputs[net->index] = net;
	}

	for (auto sig : netlist.signals) {
		match_t match(sig_cnt);
		fill(match.begin(), match.end(), nullptr);
		search_recurse(*inputs[0], *sig, match);
		bool does_match = true;
		for (auto m : match) {
			if (m == nullptr) {
				does_match = false;
				break;
			}
		}
		if (does_match)
			matches.push_back(match);

	}
	return matches;
}

//http://www.scielo.br/scielo.php?script=sci_arttext&pid=S0101-74382005000300005
// https://stackoverflow.com/questions/8176298/vf2-algorithm-steps-with-example
// https://stackoverflow.com/questions/17480142/is-there-any-simple-example-to-explain-ullmann-algorithm
//https://stackoverflow.com/questions/13537716/how-to-partially-compare-two-graphs/13537776#13537776
// https://github.com/charlieegan3/graph_match
bool Query::search_recurse(Net & refSig, Net & sig, match_t & match) {
	bool matches = false;
	// order of endpoint or drivers does (as signal is hyperedge)
	// test all combinations of paths (but with very high probability only single item
	//  as majority of the nets has only one driver)
	bool ignore_dirvers = refSig.drivers.size() == 0;
	bool ignore_endpoints = refSig.endpoints.size() == 0;

	if (ignore_dirvers) {
		if (ignore_endpoints) {
			auto & m = match[refSig.index];
			if (m == nullptr) {
				// this signal can match the reference
				m = sig;
				return true;
			} else if (m == &sig) {
				// this signal already matched this reference somewhere else
				return true;
			} else {
				// this reference is matched to different signal
				return false;
			}
		}
		for (auto refE: refSig.endpoints) {

		}
	} else {
		// vast majority of nets has only one driver
		for (auto refD : refSig.drivers) {
			for (auto candD : sig.drivers) {

			}
		}
	}
}

//void Query::search_recurse(FunctionCall & refCall, CandidateFnCall & call,
//		match_t & match) {
//
//}
//
//void Query::search_recurse(Statement & refCall, CandidateStm & call,
//		match_t & match) {
//
//}

std::pair<Query::match_t, bool> Query::find_path(iNode & a, iNode & b) {
	match_t path;
	bool found = find_path(a, b, path);
	return {path, found};
}

bool Query::find_path(iNode & a, iNode & b, vector<iNode*> & path) {
	path.push_back(&a);
	if (&a == &b)
		return true;

	for (auto node : a.forward()) {
		bool not_in_path = std::find(path.begin(), path.end(), node)
				== path.end();
		if (not_in_path) {
			auto found = find_path(*node, b, path);
			if (found)
				return true;
		}
	}
	path.pop_back();
	return false;
}

}
