#pragma once
#include <netlistDB/netlist.h>
#include <netlistDB/hw_type/common.h>
#include <netlistDB/statement_if.h>

namespace netlistDB {

class FF: public Netlist {
public:
	Net & clk;
	Net & a_in;
	Net & a_out;

	FF(hw_type::iHwType & T) :
			Netlist("ff_module"), clk(sig_in("clk", hw_type::hw_bit)), a_in(
					sig_in("a_in", T)), a_out(sig_out("a_out", T)) {
		If(clk.rising())(
			&a_out(a_in)
		);
	}
};


class FF_intern_sig: public Netlist {
public:
	Net & clk;
	Net & a_in;
	Net & a_out;

	FF_intern_sig(hw_type::iHwType & T) :
			Netlist("ff_intern_sig_module"), clk(sig_in("clk", hw_type::hw_bit)), a_in(
					sig_in("a_in", T)), a_out(sig_out("a_out", T)) {
		auto &reg = sig("reg", hw_type::hw_int32);
		If(clk.rising()) (
			&reg(a_in)
		);
		a_out(reg);
	}
};


}
