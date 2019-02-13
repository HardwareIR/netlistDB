#include <igraph.h>
#include <iostream>

#include "test_graphs.h"
#include "timer.h"

namespace netlistDB {
// https://github.com/zero-impact/Undergrad-Classes/blob/4016cc06d0269167dad0283f5780c982ba6da505/CO759/A1/examples/simple/igraph_bfs2.c

void collect_edges(const Netlist & netlist,
		std::vector<igraph_real_t> & edges) {
	for (auto node : netlist.nodes) {
		for (auto f : node->forward) {
			edges.push_back(node->index);
			edges.push_back(f->index);
			//std::cout << node->index << " -> " << f->index << std::endl;
		}
	}
}

void netlistDB_to_iGraph(igraph_t & ig, Netlist & n) {
	std::vector<igraph_real_t> edges;
	igraph_vector_t v;
	collect_edges(n, edges);
	igraph_vector_view(&v, &edges[0], edges.size());
	igraph_create(&ig, &v, 0, IGRAPH_DIRECTED);
}

}
