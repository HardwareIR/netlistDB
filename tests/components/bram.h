#pragma once
#include <netlistDB/netlist.h>
#include <netlistDB/hw_type/hw_int.h>

namespace netlistDB {

class Bram_sp: public Netlist {
public:
	hw_type::HwInt hw_int4;
	hw_type::iHwType_array hw_int32x16;
	Net & clk;
	Net & addr;
	Net & en;
	Net & d_in;
	Net & d_out;
	Net & mem;

	Bram_sp() :
			Netlist("bram_sp_module"), hw_int4(4),
			hw_int32x16(hw_type::hw_int32[16]),
			clk(sig_in("clk", hw_type::hw_bit)),
			addr(sig_in("addr", hw_int4)),
			en(sig_in("en", hw_type::hw_bit)),
			d_in(sig_in("d_in", hw_type::hw_int32)),
			d_out(sig_out("d_out", hw_type::hw_int32)),
			mem(sig("mem", hw_int32x16)) {

		If(clk.rising())({
			&d_out(mem[addr]),
			&If(en) ({
				&mem[addr](d_in),
			})
		});
	}
};

}
