#include <functional>
#include "backtrack_context.h"
#include "../statemen_if.h"

namespace netlistDB {
namespace query {
/*
 * Query to netlist
 *
 **/
class Query: public Netlist {
public:
	size_t query_size;
	std::unordered_map<Net *, size_t> signal_to_index;
	Query();

	// size of results = number of signals in this query
	// as it maps
	using match_t = std::map<iNode*, iNode*>;
	using path_t = std::vector<iNode*>;

	/**
	 * search isomorphic graphs with this query in netlist
	 * DFS
	 */

	std::vector<match_t> search(Netlist & netlist);
	static std::pair<path_t, bool> find_path(iNode & a, iNode & b);
	static bool find_path(iNode & a, iNode & b, path_t & path);

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
	static bool search_recurse(OperationNode & ref, OperationNode & n,
			BackTrackingContext & ctx);
	static bool find_matching_permutation(OrderedSet<OperationNode> & ref,
			OrderedSet<OperationNode> & graphIo, BackTrackingContext& ctx);
	static bool statements_matches(std::vector<Statement *> & ref,
			std::vector<Statement *> & n, BackTrackingContext & ctx);

};

class FloddingQuery {
	// function which select where to flood
	// <to driver, to endpoints>
	using flooding_predicate = std::function<std::pair<bool, bool>(iNode &)>;

	void search(Netlist & netlist, flooding_predicate predicate);
};

}
}

