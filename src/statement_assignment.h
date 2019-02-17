#pragma once
#include "netlist.h"

namespace netlistDB {

class Assignment: public Statement {
public:
	// the first index to apply is the last in this vector
	std::vector<Net*> dst_index;
	Net & dst;
	Net & src;

	Assignment(const Assignment & other) = delete;
	Assignment(Net & dst, Net & src);
protected:
	/* Find out if dst signal is something indexed
	 * save indexes in to dst_index
	 *
	 * @return clean dst net without any index operator
	 */
	Net & extract_dst_index_cascade(Net & dst);
};

}
