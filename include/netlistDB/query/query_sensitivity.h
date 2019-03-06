#pragma once

#include <netlistDB/netlist.h>
#include <netlistDB/statement_assignment.h>
#include <netlistDB/statement_if.h>

namespace netlistDB {
namespace query {

class NETLISTDB_PUBLIC QuerySensitivity {
public:
	/*
	 * Discover the sensitivity of all statements
	 * */
	static void apply(Netlist & ctx);
	static void apply(Statement & stm, std::set<iNode*> & seen);
	static void apply(Assignment & assig, std::set<iNode*> & seen);
	static void apply(IfStatement & ifstm, std::set<iNode*> & seen);

};

}
}
