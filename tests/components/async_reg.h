#pragma once
#include <netlistDB/netlist.h>
#include <netlistDB/hw_type/common.h>
#include <netlistDB/statement_if.h>

namespace netlistDB {

class FF_async_rst: public Netlist {
public:
	Net & clk;
	Net & async_rst;
	Net & a_in;
	Net & a_out;

	FF_async_rst(hw_type::iHwType & T) :
			Netlist("FF_async_rst"), clk(sig_in("clk", hw_type::hw_bit)),
			async_rst(sig_in("async_rst", hw_type::hw_bit)),
			a_in(sig_in("a_in", T)), a_out(sig_out("a_out", T)) {

		If(async_rst)(
			&a_out(0)
		).Elif(clk.rising(),
			&a_out(a_in)
		);
	}
};

class FF_async_rstn: public Netlist {
public:
	Net & clk;
	Net & async_rstn;
	Net & a_in;
	Net & a_out;

	FF_async_rstn(hw_type::iHwType & T) :
			Netlist("FF_async_rstn"), clk(sig_in("clk", hw_type::hw_bit)),
			async_rstn(sig_in("async_rstn", hw_type::hw_bit)),
			a_in(sig_in("a_in", T)), a_out(sig_out("a_out", T)) {

		If(~async_rstn)(
			&a_out(0)
		).Elif(clk.rising(),
			&a_out(a_in)
		);
	}
};


}
