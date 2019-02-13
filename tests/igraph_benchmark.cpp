#include <igraph.h>
#include <iostream>

#include "test_graphs.h"
#include "timer.h"
#include "netlist_to_igraph.h"

using namespace netlistDB;

/* Generate dummy igraph with some edges
 * */
void dummy_igraph(igraph_t & graph) {
	igraph_vector_t v;
	igraph_real_t edges[] = { 0, 1, 0, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 7, 0, 8, 0,
			10, 0, 11, 0, 12, 0, 13, 0, 17, 0, 19, 0, 21, 0, 31, 1, 2, 1, 3, 1,
			7, 1, 13, 1, 17, 1, 19, 1, 21, 1, 30, 2, 3, 2, 7, 2, 27, 2, 28, 2,
			32, 2, 9, 2, 8, 2, 13, 3, 7, 3, 12, 3, 13, 4, 6, 4, 10, 5, 6, 5, 10,
			5, 16, 6, 16, 8, 30, 8, 32, 8, 33, 9, 33, 13, 33, 14, 32, 14, 33,
			15, 32, 15, 33, 18, 32, 18, 33, 19, 33, 20, 32, 20, 33, 22, 32, 22,
			33, 23, 25, 23, 27, 23, 32, 23, 33, 23, 29, 24, 25, 24, 27, 24, 31,
			25, 31, 26, 29, 26, 33, 27, 33, 28, 31, 28, 33, 29, 32, 29, 33, 30,
			32, 30, 33, 31, 32, 31, 33, 32, 33 };
	igraph_vector_view(&v, edges, sizeof(edges) / sizeof(double));
	igraph_create(&graph, &v, 0, IGRAPH_DIRECTED);
}

igraph_bool_t bfs_callback(const igraph_t *graph,
			   igraph_integer_t vid,
			   igraph_integer_t pred,
			   igraph_integer_t succ,
			   igraph_integer_t rank,
			   igraph_integer_t dist,
			   void *extra) {
  //printf(" %li", (long int) vid);
	//usleep(1);
	(*reinterpret_cast<size_t*>(extra))++;
	return 0;
}

void igraph_bfs_benchmark(const igraph_t & graph, const Netlist & netlist) {
	std::vector<Net*> outputs;
	for (auto & n : netlist.nets) {
		if (n->direction == Direction::DIR_OUT) {
			outputs.push_back(n);
		}
	}
	igraph_vector_t roots;
	igraph_vector_init(&roots, outputs.size());
	for (size_t i = 0; i < outputs.size(); i++) {
		VECTOR(roots)[i]=outputs[i]->index;
	}
	size_t visited = 0;
	auto t = new Timer("igraph_bfs");
	igraph_bfs(&graph, /*root=*/-1, /*roots=*/ &roots, /*neimode=*/ IGRAPH_OUT,
		     /*unreachable=*/ 1, /*restricted=*/ 0,
		     /*order*/nullptr, /*rank*/ nullptr, /*father*/nullptr,
			 /*pred*/nullptr, /*succ*/nullptr, /*dist*/ nullptr,
			 /*callback=*/ &bfs_callback, /*extra=*/ &visited);
	delete t;
	assert(visited == netlist.nodes.size());
}

int main(void) {
	igraph_t graph;
	igraph_vector_t result;

	Netlist ctx("example");
	std::mt19937 rand(0);
	size_t N = 100;
	build_random_circuit(ctx, N, N, N, N, rand);
	netlistDB_to_iGraph(graph, ctx);
	//dummy_igraph(graph);

	igraph_vector_init(&result, 0);
	igraph_degree(&graph, &result, igraph_vss_all(), IGRAPH_ALL, IGRAPH_LOOPS);
	printf("Maximum degree is      %10i, vertex %2i.\n",
			(int) igraph_vector_max(&result),
			(int) igraph_vector_which_max(&result));
	igraph_bfs_benchmark(graph, ctx);
	igraph_vector_destroy(&result);
	igraph_destroy(&graph);

	return 0;
}
