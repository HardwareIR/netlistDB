#include <netlistDB/serializer/verilog.h>

using namespace std;

namespace netlistDB {
namespace serializer {

Verilog2001::Verilog2001(
		std::map<const std::string, const void*> reserved_names) :
		name_scope(true), indent_cnt(0) {
	name_scope.set_level(1);
	auto & scope = *name_scope[0];
	for (auto & kw : keywords) {
		scope[kw] = new KeyWord;
	}
}

void Verilog2001::serialize_net_usage(const Net & n, std::ostream & str) {
	if (n.id.hidden) {
		if (n.val) {
			assert(n.drivers.size() == 0);
			throw std::runtime_error(
					std::string("Not implemented ") + __PRETTY_FUNCTION__);
		} else {
			assert(n.drivers.size() == 1);
			auto op = dynamic_cast<const FunctionCall*>(n.drivers[0]);
			assert(n.drivers.size() == 1);
			assert(op);
			serialize(*op, str);
		}
	} else {
		str << name_scope.checkedName(n.id.name, &n);
	}
}

Verilog2001::~Verilog2001() {
	auto & scope = *name_scope[0];
	for (auto & kw : keywords) {
		delete reinterpret_cast<const KeyWord*>(scope[kw]);
	}
}

void Verilog2001::serialize_block(const vector<Statement*> & stms,
		std::ostream & str) {
	if (stms.size() == 1) {
		str << endl;
		indent_cnt++;
		Serializer::serialize(*stms[0], str);
		str << endl;
		indent_cnt--;
	} else {
		str << " begin" << std::endl;

		indent_cnt++;
		for (auto s : stms) {
			Serializer::serialize(*s, str);
			str << endl;
		}
		indent_cnt--;

		indent(str) << "end";
	}
}

std::ostream & Verilog2001::indent(std::ostream & str) {
	for (size_t i = 0; i < indent_cnt; i++) {
		str << INDENT;
	}
	return str;
}

enum Verilog2001::VERILOG_NET_TYPE Verilog2001::verilogTypeOfSig(
		const Net & n) const {
	size_t driver_cnt = n.drivers.size();
	if (n.direction == Direction::DIR_IN) {
		// is input port
		return VERILOG_NET_TYPE::VERILOG_WIRE;
	} else if (driver_cnt == 1) {
		auto d = dynamic_cast<Assignment*>(n.drivers[0]);
		if (d and not d->parent and not d->dst_index.size()
				and not d->sens.now_is_event_dependent and not d->src.id.hidden)
			// primitive assignment
			return VERILOG_NET_TYPE::VERILOG_WIRE;
	}

	return VERILOG_NET_TYPE::VERILOG_REG;
}

const std::map<const FunctionDef*, int> & Verilog2001::get_operator_precedence() {
	return opPrecedence;
}


}
}
