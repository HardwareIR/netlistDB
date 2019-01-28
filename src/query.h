#include <functional>
#include "netlist.h"

namespace netlistDB {

/*
 * Query to netlist
 *
 **/
class Query: public Netlist {
public:
	size_t query_size;
	std::unordered_map<Net *, size_t> signal_to_index;
	Query();

	// signal from target graph
	using CandidateSig = Net;
	using CandidateFnCall = FunctionCall;
	using CandidateStm = Statement;

	// size of results = number of signals in this query
	// as it maps
	using match_t = std::vector<CandidateSig*>;

	/**
	 * search isomorphic graphs with this query in netlist
	 *
	 */
	std::vector<match_t> search(Netlist & netlist);

	void search_recurse(Net & refSig, CandidateSig & sig, match_t & match);
	void search_recurse(FunctionCall & refCall, CandidateFnCall & call, match_t & match);
	void search_recurse(Statement & refCall, CandidateStm & call, match_t & match);

};

class FloddingQuery {
	// function which select where to flood
	// <to driver, to endpoints>
	using flooding_predicate = std::function<std::pair<bool, bool>(Net &)>;

	void search(Netlist & netlist, flooding_predicate predicate);
};

}

