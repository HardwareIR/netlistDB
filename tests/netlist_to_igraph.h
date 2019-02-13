#pragma once
#include <igraph.h>
#include <iostream>
#include "../src/netlist.h"


namespace netlistDB {
/*
 * Collect edges from Netlist to vector suitable for igraph_t constructor
 **/
void collect_edges(const Netlist & netlist,
		std::vector<igraph_real_t> & edges);

/* Convert Netlist to igraph_t
 **/
void netlistDB_to_iGraph(igraph_t & ig, Netlist & n);

}
