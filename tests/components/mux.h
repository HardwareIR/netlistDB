#pragma once
#include <netlistDB/netlist.h>

namespace netlistDB {


class Mux4x32: public Netlist {
public:
	Net & sel;
	Net & a_in0;
	Net & a_in1;
	Net & a_in2;
	Net & a_in3;
	Net & a_out;

	Mux4x32() :
			Netlist("mux4x32_module"),
			sel(sig_in("sel", hw_type::hw_uint8)),
			a_in0(sig_in("a_in0", hw_type::hw_int32)),
			a_in1(sig_in("a_in1", hw_type::hw_int32)),
			a_in2(sig_in("a_in2", hw_type::hw_int32)),
			a_in3(sig_in("a_in3", hw_type::hw_int32)),
			a_out(sig_out("a_out", hw_type::hw_int32)) {

		If(sel == 0) (&a_out(a_in0))
		.Elif(sel == 1, &a_out(a_in1))
		.Elif(sel == 2, &a_out(a_in2))
		.Elif(sel == 3, &a_out(a_in3))
		.Else(&a_out(0));
	}
};

}
