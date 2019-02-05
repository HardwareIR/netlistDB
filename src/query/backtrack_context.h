#pragma once
#include "../netlist.h"

namespace netlistDB {
namespace query {

/*
 * The context used for management of the matched object
 * during graph pattern-match
 **/
class BackTrackingContext;
class BackTrackingContext {
public:
	// query: graph
	std::map<iNode*, iNode*> & match;
	// the nodes from query graph which were matched in this context
	// and should be removed if the context is discarded
	std::set<iNode*> private_match;
	// children matches which should be discarded if this context is discarded
	std::vector<BackTrackingContext*> children;

	BackTrackingContext(std::map<iNode*, iNode*> & match);

	/*
	 * Create a child in this BackTrackingContext
	 **/
	BackTrackingContext & child();

	/*
	 * @return true if "c" is children of this BackTrackingContext (recursively)
	 **/
	bool isMyChild(BackTrackingContext* c);

	// discard all matches discovered in this branch
	void discard();

	enum match_check_t {
		already_matches, can_match, can_not_match,
	};

	/*
	 * Check if the object from target graph "n" can match the object from the query "ref"
	 *
	 * @param ref object from the query
	 * @param n object from the target graph
	 **/
	match_check_t check_can_match(iNode & ref, iNode & n);

	/* Set object from the target graph "n" to match the object from query graph "ref"
	 * @param ref object from the query
	 * @param n object from the target graph
	 **/
	void insert_match(iNode & ref, iNode & n);

	// destroy last child BackTrackingContext
	void pop_child();

	~BackTrackingContext();
};
}

}
