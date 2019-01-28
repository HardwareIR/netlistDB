#pragma once

#include <iostream>
#include "../netlist.h"

namespace netlistDB {
namespace serializer {

class Verilog2001 {
	Verilog2001();
	virtual void serialize(Netlist & netlist, std::ostream & str);

};

}
}
