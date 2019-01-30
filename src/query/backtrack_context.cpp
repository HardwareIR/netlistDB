#include "backtrack_context.h"

namespace netlistDB {
namespace query {

BackTrackingContext::BackTrackingContext(std::map<iNode*, iNode*> & match) :
		match(match) {
}

BackTrackingContext & BackTrackingContext::child() {
	auto * ch = new BackTrackingContext(match);
	children.push_back(ch);
	return *ch;
}

bool BackTrackingContext::isMyChild(BackTrackingContext* c) {
	if (c == nullptr)
		return false;

	for (auto ch : children) {
		if (ch == c or ch->isMyChild(c))
			return true;
	}
	return false;
}

void BackTrackingContext::discard() {
	for (auto ch : children)
		delete ch;

	for (auto item : private_match) {
		match.erase(item);
	}

	children.clear();
}

BackTrackingContext::match_check_t BackTrackingContext::check_can_match(
		iNode & ref, iNode & n) {
	auto m = match.find(&ref);
	if (m != match.end() and m->second == &n) {
		// is already matched to this object
		return already_matches;
	}

	if (m == match.end()) {
		return can_match;
	}

	return can_not_match;
}

void BackTrackingContext::insert_match(iNode & ref, iNode & n) {
	if(match.find(&ref) == match.end()) {
		private_match.insert(&ref);
		match[&ref] = &n;
	}
}

void BackTrackingContext::pop_child() {
	auto ch = children.at(children.size() - 1);
	delete ch;
	children.pop_back();
}

BackTrackingContext::~BackTrackingContext() {
	for (auto ch : children) {
		delete ch;
	}
}

}
}
