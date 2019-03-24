#pragma once
#include <netlistDB/netlist.h>
#include <netlistDB/hw_type/common.h>
#include <netlistDB/statement_if.h>

namespace netlistDB {

class Latch: public Netlist {
public:
	Net & en;
	Net & a_in;
	Net & a_out;

	Latch(hw_type::iHwType & T) :
			Netlist("latch_module"), en(sig_in("en", hw_type::hw_bit)), a_in(
					sig_in("a_in", T)), a_out(sig_out("a_out", T)) {
		If(en)(
			&a_out(a_in)
		);
	}
};

}
