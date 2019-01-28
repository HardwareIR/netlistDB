#pragma once
#include <exception>
#include <string>

namespace netlistDB {
namespace serializer {

class NameOccupiedErr: public std::runtime_error {
	void * usedOn;
	NameOccupiedErr(void *usedOn);
};

class NameScope;
/*
 * if name is discovered in scope it is converted to name_id
 * where id is sequential number for prefix name\_
 */
class NameScopeItem {
	size_t myLvl;

	NameScopeItem(size_t myLvl);
	NameScope * get_child(NameScope & parent);
	NameScope * get_parent(NameScope & parent);
	std::string __incrPrefixCntrsForChilds(const std::string & prefix,
			const std::string & currentVal, NameScope & parent);
	void __registerName(const std::string & name, void *obj,
			NameScope & parent);
	std::string get_usable_name(const std::string & suggestedName, void * obj,
			NameScope & parent);
};

//Scope of used names in hdl
class NameScope {

	NameScope(bool ignorecase);
	NameScope & fork(size_t lvl);

	/**
	 *	Trim or extend scope
	 *	lvl = 1 -> only one scope (global)
	 **/
	void set_level(size_t lvl);

	std::string checkedName(const std::string & actualName, void * actualObj,
			bool isGlobal = false);
};
}
}
