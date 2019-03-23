#include <netlistDB/serializer/namescope.h>
#include <algorithm>
#include <assert.h>

using namespace std;
namespace netlistDB {
namespace serializer {

NameScopeItem::NameScopeItem(size_t myLvl) :
		myLvl(myLvl) {
}

NameScopeItem * NameScopeItem::get_child(NameScope & parent) {
	if (parent.size() >= myLvl + 1) {
		return nullptr;
	} else {
		return parent[myLvl + 1];
	}
}

NameScopeItem * NameScopeItem::get_parent(NameScope & parent) {
	if (myLvl < 1) {
		return nullptr;
	} else {
		return parent.at(myLvl - 1);
	}
}

string NameScopeItem::__incrPrefixCntrsForChilds(const string & prefix,
		int currentVal, NameScope & parent) {
	// [TODO] check if new name is not defined in any direction
	currentVal += 1;
	cntrsForPrefixNames[prefix] = currentVal;
	NameScopeItem * ch = get_child(parent);
	while (ch) {
		auto f = ch->cntrsForPrefixNames.find(prefix);
		if (f != ch->cntrsForPrefixNames.end()) {
			ch->cntrsForPrefixNames[prefix] = currentVal;
			ch = ch->get_child(parent);
		} else {
			// prefix is not registered at any child
			break;
		}
	}
	auto usableName = prefix + to_string(currentVal);
	return usableName;
}

void NameScopeItem::__registerName(const string & name, const void *obj,
		NameScope & parent) {
	// search if name is already defined on me and parents
	NameScopeItem * actual = this;
	const void * o = nullptr;

	string _name = name;
	if (parent.ignorecase) {
		transform(_name.begin(), _name.end(), _name.begin(), ::tolower);
	}

	while (actual) {
		auto f = actual->find(_name);
		if (f != actual->end()) {
			o = f->second;
			break;
		} else {
			actual = actual->get_parent(parent);
		}
	}

	if (o == nullptr or o == obj) {
		// we can use use the name, because it is not used
		(*this)[_name] = obj;
	} else {
		throw NameOccupiedErr(o);
	}
}

string NameScopeItem::get_usable_name(const string & _suggestedName,
		const void * obj, NameScope & parent) {
	assert(
			_suggestedName.size() > 0
					&& "at least some part of name has to be specified");
	string suggestedName = _suggestedName;
	if (suggestedName.back() != '_') {
		try {
			__registerName(suggestedName, obj, parent);
			return suggestedName;
		} catch (const NameOccupiedErr &) {
			suggestedName += "_";
		}
	}

	NameScopeItem * actual = this;
	auto f = actual->cntrsForPrefixNames.find(suggestedName);
	int cntrVal = -1;
	if (f != actual->cntrsForPrefixNames.end()) {
		cntrVal = f->second;
	}

	// setup for me and propagate to children
	auto usableName = __incrPrefixCntrsForChilds(suggestedName, cntrVal,
			parent);
	__registerName(usableName, obj, parent);
	return usableName;
}

NameScope::NameScope(bool ignorecase) :
		ignorecase(ignorecase), all_names(_all_names) {
}

NameScope::NameScope(bool ignorecase,
		std::map<const void*, std::string> & all_names) :
		ignorecase(ignorecase), all_names(all_names) {
}

NameScope * NameScope::fork(size_t lvl) {
	auto f = new NameScope(ignorecase);
	f->resize(size());
	copy(this->begin(), this->end(), f->begin());
	return f;
}

void NameScope::set_level(size_t lvl) {
	while (size() != lvl) {
		if (size() > lvl) {
			level_pop();
		} else {
			level_push();
		}
	}
}

void NameScope::level_pop() {
	delete back();
	pop_back();
}

void NameScope::level_push() {
	push_back(new NameScopeItem(size()));
}

string NameScope::checkedName(const string & actualName, const void * actualObj,
		bool isGlobal) {
	auto f = all_names.find(actualObj);
	if (f != all_names.end()) {
		// the name was already checked
		return f->second;
	}

	NameScopeItem * ch;
	if (isGlobal) {
		ch = front();
	} else {
		ch = back();
	}
	auto n = ch->get_usable_name(actualName, actualObj, *this);
	all_names[actualObj] = n;
	return n;
}

NameScope::~NameScope() {
	while (this->size()) {
		delete back();
		pop_back();
	}
}

}
}
