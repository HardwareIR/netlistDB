#pragma once
#include <string>

namespace netlistDB {
// definition of function (operator is also function)
class FunctionDef {
public:
	std::string name;
	size_t arg_cnt;

	FunctionDef(const FunctionDef & other) = delete;
	FunctionDef(const std::string & name, size_t arg_cnt);

};

}
