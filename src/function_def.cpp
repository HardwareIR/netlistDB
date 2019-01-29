#include "netlist.h"

using namespace netlistDB;

FunctionDef::FunctionDef(const std::string & name, size_t arg_cnt) :
		name(name), arg_cnt(arg_cnt) {
}

