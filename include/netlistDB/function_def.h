#pragma once

#include <string>
#include <netlistDB/common.h>

namespace netlistDB {
// definition of function (operator is also function)
class NETLISTDB_PUBLIC FunctionDef {
public:
	std::string name;
	size_t arg_cnt;

	FunctionDef(const FunctionDef & other) = delete;
	FunctionDef(const std::string & name, size_t arg_cnt);

};

}
