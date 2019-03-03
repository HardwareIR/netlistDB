#pragma once
#include <exception>
#include <string>
#include <vector>
#include <map>

namespace netlistDB {
namespace serializer {

class NameOccupiedErr: public std::runtime_error {
public:
	const void * usedOn;
	NameOccupiedErr(const void *usedOn) :
			std::runtime_error("NameOccupiedErr"), usedOn(usedOn) {
	}
};

// dummy object to reserver name for keyword
class KeyWord {
};

class NameScope;
/*
 * If name is discovered in scope it is converted to name_id
 * where id is sequential number for prefix name\_
 */
class NameScopeItem: public std::map<const std::string, const void*> {
	size_t myLvl;

	/* some names are specified just as prefix and serializer
	 * should resolve correct name for object
	 * this happens for most of generated objects
	 */
	std::map<const std::string, int> cntrsForPrefixNames;

public:
	NameScopeItem(size_t myLvl);
	NameScopeItem * get_child(NameScope & parent);
	NameScopeItem * get_parent(NameScope & parent);
	std::string __incrPrefixCntrsForChilds(const std::string & prefix,
			int currentVal, NameScope & parent);
	void __registerName(const std::string & name, const void *obj,
			NameScope & parent);

	/*
	 * Get name for the object which does not collide with anything else
	 *
	 * @param suggested_name the desired name for this object
	 * @param obj the object which is the name for
	 * @param parent the parent NameScope object
	 * */
	std::string get_usable_name(const std::string & suggestedName,
			const void * obj, NameScope & parent);
};

// Scope of used names in HDL to prevent name collision
class NameScope: public std::vector<NameScopeItem*> {
public:
	bool ignorecase;

	NameScope(bool ignorecase);
	NameScope * fork(size_t lvl);

	/**
	 *	Trim or extend scope
	 *	lvl = 1 -> only one scope (global)
	 **/
	void set_level(size_t lvl);

	std::string checkedName(const std::string & actualName,
			const void * actualObj, bool isGlobal = false);
};

}
}
