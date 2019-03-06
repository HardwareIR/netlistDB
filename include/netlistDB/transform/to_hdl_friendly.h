#pragma once

#include <netlistDB/transform/transformation.h>

namespace netlistDB {
namespace transform {

/* Transform the circuit in format suitable for serialization.
 * The output format tries to be human readable and is deterministic thus also suitable for git.
 *
 * 1. remove code duplication
 * 2. update id.hidden flags
 * 3. wrap statements to HwProcess instances
 **/
class NETLISTDB_PUBLIC TransformToHdlFriendly: public iTransformation {
public:
	virtual bool apply(Netlist & ctx) override;
};

}
}
