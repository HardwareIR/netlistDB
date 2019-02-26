#pragma once

#include <iostream>
#include <netlistDB/netlist.h>
#include <netlistDB/serializer/serializer.h>

namespace netlistDB {
namespace serializer {

class Verilog2001: public Serializer {
public:
	Verilog2001();
	virtual void serialize(Netlist & netlist, std::ostream & str) override;
	void indent(size_t cnt, std::ostream & str);

};

}
}
