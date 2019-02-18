#pragma once
#include <netlistDB/transform/transformation.h>

namespace netlistDB {
namespace transform {

/*
 * Transform a[0] = b; a[1] = c; to a = {b, c}
 **/
class TransformIndexedAssignmentsToAssignmentOfConcat: iTransformation {
protected:

	// the range is closed <0, 9> is 10 bits
	using range_and_src_t = std::pair<std::pair<size_t, size_t>, Net*>;
	bool is_result_of_indexed_assignment(Net & sig, std::vector<Net*> & inputs);

public:
	/*
	 * Apply transformation to netlist
	 *
	 * @return true if circuit was modified
	 **/
	virtual bool apply(Netlist & ctx) override;
};

}
}
