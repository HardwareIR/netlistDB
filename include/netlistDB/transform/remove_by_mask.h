#pragma once

#include <netlistDB/transform/transformation.h>
#include <netlistDB/query/query_traverse.h>
#include <netlistDB/utils/thread_pool.h>

namespace netlistDB {
namespace transform {

class TransformRemoveByMask: iTransformation {
	typename query::QueryTraverse::flag_t * node_to_keep_mask;
	utils::ThreadPool thread_pool;
public:
	/*
	 * @param node_to_keep_mask mask of same size as nodes in Netlist
	 *      if item is false the node on this index will be removed
	 **/
	TransformRemoveByMask(
			typename query::QueryTraverse::flag_t node_to_keep_mask[]);

	/*
	 * Apply transformation to netlist
	 * @return true if circuit was modified
	 **/
	virtual bool apply(Netlist & ctx) override;
};

}
}
