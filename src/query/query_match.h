#include <functional>
#include "backtrack_context.h"
#include "../statemen_if.h"

namespace netlistDB {
namespace query {

/*
 * Graph pattern match query
 *
 **/
class QueryMatch: public Netlist {
public:
	size_t query_size;
	std::unordered_map<Net *, size_t> signal_to_index;
	QueryMatch();

	// size of results = number of signals in this query
	// as it maps
	using match_t = std::map<iNode*, iNode*>;

	/**
	 * search isomorphic graphs with this query in netlist
	 * DFS
	 */

	std::vector<match_t> search(Netlist & netlist);

protected:
	static bool search_recurse(FunctionCall & ref, FunctionCall & call,
			BackTrackingContext & match);
	static bool search_recurse(iNode & ref, iNode & net,
			BackTrackingContext & match);
	static bool search_recurse(Net & ref, Net & net,
			BackTrackingContext & match);
	static bool search_recurse(Assignment & ref, Assignment & assig,
			BackTrackingContext & match);
	static bool search_recurse(IfStatement & ref, IfStatement & ifstm,
			BackTrackingContext & match);
	static bool find_matching_permutation(OrderedSet<OperationNode*> & ref,
			OrderedSet<OperationNode*> & graphIo, BackTrackingContext& ctx);
	static bool statements_matches(std::vector<Statement *> & ref,
			std::vector<Statement *> & n, BackTrackingContext & ctx);

};

}
}

