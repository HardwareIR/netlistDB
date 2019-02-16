#pragma once
#include "transformation.h"
#include "../query/query_traverse.h"

namespace netlistDB {
namespace transform {

class TransformRemoveByMask: iTransformation {
	typename query::QueryTraverse::flag_t * node_to_keep_mask;

public:
	/*
	 * @param node_to_keep_mask mask of same size as nodes in Netlist
	 *      if item is false the node on this index will be removed
	 **/
	TransformRemoveByMask(typename query::QueryTraverse::flag_t node_to_keep_mask[]);

	/*
	 * Apply transformation to netlist
	 * @param thread_cnt the number of threads to use if 0 the optimal number will be used
	 * @return true if circuit was modified
	 **/
	virtual bool apply(Netlist & ctx, size_t thread_cnt = 0) override;
};

}
}
