#pragma once

#include <iostream>
#include "../netlist.h"
#include "serializer.h"

namespace netlistDB {
namespace serializer {

class Verilog2001: public Serializer {
public:
	Verilog2001();
	virtual void serialize(Netlist & netlist, std::ostream & str) override;

};

}
}
