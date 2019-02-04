#include "netlist.h"
#include "statemen_if.h"

using namespace netlistDB;

Netlist::Netlist(const std::string & name) :
		name(name), obj_seq_num(0) {
}

Net & Netlist::sig_in(const std::string & name) {
	auto s = new Net(*this, obj_seq_num++, name, DIR_IN);
	nets.insert(s);
	return *s;
}

Net & Netlist::sig_out(const std::string & name) {
	auto s = new Net(*this, obj_seq_num++, name, DIR_OUT);
	nets.insert(s);
	return *s;
}

Net & Netlist::sig() {
	return sig("");
}

Net & Netlist::sig(const std::string & name) {
	auto s = new Net(*this, obj_seq_num++, name, DIR_UNKNOWN);
	nets.insert(s);
	return *s;
}

Netlist::~Netlist() {
	std::set<void *> deleted;
	for (auto sig : nets) {
		for (auto & d : sig->drivers) {
			auto fn = dynamic_cast<FunctionCall*>(d);
			if (fn and &(fn->res) == sig) {
				delete fn;
				continue;
			}
			auto asAssig = dynamic_cast<Assignment*>(d);
			if (asAssig and asAssig->parent == nullptr) {
				if (&(asAssig->dst) == sig) {
					deleted.insert(asAssig);
					delete asAssig;
				}
				continue;
			}
		}

		for (auto & ep : sig->endpoints) {
			if (deleted.find(ep) == deleted.end())
				continue;

			auto asIf = dynamic_cast<IfStatement*>(ep);
			if (asIf) {
				if (&(asIf->condition) == sig) {
					delete ep;
				}
			}
		}
	}
}
