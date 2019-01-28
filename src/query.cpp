#include "query.h"

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

	vector<Signal*> inputs(sig_cnt);
	inputs.resize(sig_cnt);
	fill(inputs.begin(), inputs.end(), nullptr);
	for (auto & sig : signals) {
		inputs[sig->index] = sig;
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
void Query::search_recurse(Signal & refSig, CandidateSig & sig,
		match_t & match) {
	// order of endpoint or drivers does (as signal is hyperedge)
	if (refSig.drivers.size() != 0) {

	}

	if (refSig.endpoints.size() != 0) {

	}
}

void Query::search_recurse(FunctionCall & refCall, CandidateFnCall & call,
		match_t & match) {

}

void Query::search_recurse(Statement & refCall, CandidateStm & call,
		match_t & match) {

}

}
