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

	// size of results = number of signals in this query
	// as it maps
	using match_t = std::vector<iNode*>;

	/**
	 * search isomorphic graphs with this query in netlist
	 * DFS
	 */
	std::vector<match_t> search(Netlist & netlist);
	std::vector<match_t> search();


	static std::pair<match_t, bool> find_path(iNode & a, iNode & b);
	static bool find_path(iNode & a, iNode & b, std::vector<iNode*> & path);

};

class FloddingQuery {
	// function which select where to flood
	// <to driver, to endpoints>
	using flooding_predicate = std::function<std::pair<bool, bool>(iNode &)>;

	void search(Netlist & netlist, flooding_predicate predicate);
};

}

