#include <functional>
#include "netlist.h"

namespace netlistDB {

/*
 * Query to netlist
 *
 **/
class Query: public Netlist {
	size_t query_size;
	std::unordered_map<Signal *, size_t> signal_to_index;

	// signal from target graph
	using CandidateSig = Signal;
	using CandidateFnCall = FunctionCall;
	using CandidateStm = Statement;

	// size of results = number of signals in this query
	// as it maps
	using result_t = std::vector<CandidateSig*>;

	/**
	 * search isomorphic graphs with this query in netlist
	 *
	 */
	std::vector<result_t> search(Netlist & netlist);

	void search_recurse(Signal & refSig, CandidateSig & sig);
	void search_recurse(FunctionCall & refCall, CandidateFnCall & call);
	void search_recurse(Statement & refCall, CandidateStm & call);

};

class FloddingQuery {
	// function which select where to flood
	// <to driver, to endpoints>
	using flooding_predicate = std::function<std::pair<bool, bool>(Signal &)>;

	void search(Netlist & netlist, flooding_predicate predicate);
};

}

