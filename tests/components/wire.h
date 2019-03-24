#pragma once
#include <netlistDB/netlist.h>

namespace netlistDB {

class Wire: public Netlist {
public:
	Net & a_in;
	Net & a_out;

	Wire(hw_type::iHwType & T) :
			Netlist("wire_module"), a_in(sig_in("a_in", T)), a_out(
					sig_out("a_out", T)) {
		a_out(a_in);
	}
};

}
