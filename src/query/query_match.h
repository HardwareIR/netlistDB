#include <functional>

#include "../statement_if.h"
#include "../statement_assignment.h"
#include "backtrack_context.h"

namespace netlistDB {
namespace query {

/*
 * Graph pattern match query
 *
 **/
class QueryMatch: public Netlist {
public:
	QueryMatch();

	// size of results = number of signals in this query
	// as it maps
	using match_t = std::map<iNode*, iNode*>;

	/**
	 * search isomorphic graphs with this query in netlist
	 * DFS based
	 */
	std::vector<match_t> search(Netlist & netlist);

protected:
	static bool search_recurse(FunctionCall & ref, FunctionCall & call,
			BackTrackingContext & match);
	static bool search_recurse(iNode & ref, iNode & net,
			BackTrackingContext & match);
	/**
	 * Signal matches if each of its connected nodes matches in any order
	 * Node matches if it has same type and structure and all of its connected
	 * signals matches in same order
	 *
	 * @param ref the net from the query
	 * @param net the net from the queried graph
	 * @return true if match was found
	 **/
	static bool search_recurse(Net & ref, Net & net,
			BackTrackingContext & match);
	static bool search_recurse(Assignment & ref, Assignment & assig,
			BackTrackingContext & match);
	static bool search_recurse(IfStatement & ref, IfStatement & ifstm,
			BackTrackingContext & match);
	/**
	 * Find first matching combination
	 *
	 * @note The match is stored in ctx.match and can be canceled by methods of ctx
	 * @param ref set of operations from query
	 * @param graphIo set of operations from the queried graph
	 * @param allow_more_in_graph if true there can an extra items in the graph
	 * @return true if match was found
	 **/
	static bool find_matching_permutation(OrderedSet<OperationNode*> & ref,
			OrderedSet<OperationNode*> & graphIo, BackTrackingContext& ctx,
			bool allow_more_in_graph);

	static bool statements_matches(std::vector<Statement *> & ref,
			std::vector<Statement *> & n, BackTrackingContext & ctx);
};

}
}

