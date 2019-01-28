#include "netlist.h"
#include "statemen_if.h"

using namespace netlistDB;

Netlist::Netlist(const std::string & name) :
		name(name), signal_seq_num(0) {
}

Net & Netlist::sig_in(const std::string & name) {
	auto s = new Net(*this, signal_seq_num++, name, DIR_IN);
	signals.insert(s);
	return *s;
}

Net & Netlist::sig_out(const std::string & name) {
	auto s = new Net(*this, signal_seq_num++, name, DIR_OUT);
	signals.insert(s);
	return *s;
}

Net & Netlist::sig() {
	return sig("");
}

Net & Netlist::sig(const std::string & name) {
	auto s = new Net(*this, signal_seq_num++, name, DIR_UNKNOWN);
	signals.insert(s);
	return *s;
}

Netlist::~Netlist() {
	for (auto sig : signals) {
		for (auto & d : sig->drivers) {
			if (d.isFn() and &(d.fnCall->res) == sig)
				delete d.fnCall;
		}

		for (auto & ep : sig->endpoints) {
			if (ep.isStm()) {
				auto asIf = dynamic_cast<IfStatement*>(ep.stm);
				if (&(asIf->condition) == sig) {
					delete ep.stm;
				}
			}
		}
	}
}
