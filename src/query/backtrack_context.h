#pragma once
#include "../netlist.h"

namespace netlistDB {
namespace query {

class BackTrackingContext;
// reference node from query -> <owner who discovered node, node>
//using owned_match_t = std::map<iNode*, std::pair<BackTrackingContext*, iNode*>>;

class BackTrackingContext {
public:
	// query: graph
	std::map<iNode*, iNode*> & match;
	// query
	std::set<iNode*> private_match;

	std::vector<BackTrackingContext*> children;

	BackTrackingContext(std::map<iNode*, iNode*> & match);
	BackTrackingContext & child();
	bool isMyChild(BackTrackingContext* c);
	// discard all matches discovered in this branch
	void discard();

	enum match_check_t {
		already_matches, can_match, can_not_match,
	};

	match_check_t check_can_match(iNode & ref, iNode & n);
	void insert_match(iNode & ref, iNode & n);
	void pop_child();

	~BackTrackingContext();
};
}

}
