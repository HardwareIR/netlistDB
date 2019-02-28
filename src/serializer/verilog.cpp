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

void Verilog2001::serialize_module_head(const Netlist & netlist,
		ostream & str) {
}

void Verilog2001::serialize_module_body(const Netlist & netlist,
		ostream & str) {

}

void Verilog2001::serialize(const Netlist & netlist, ostream & str) {
	serialize_module_head(netlist, str);
	serialize_module_body(netlist, str);
}

Verilog2001::~Verilog2001() {
	auto & scope = *name_scope[0];
	for (auto & kw : keywords) {
		delete reinterpret_cast<const KeyWord*>(scope[kw]);
	}
}

void Verilog2001::serialize_component_instance(const std::string & module_name,
		const std::string & instance_name, std::vector<Net*> & params,
		std::map<Net*, Net*> param_map, std::vector<Net*> & io,
		std::map<Net*, Net*> io_map, std::ostream & str) {
	/*
	 *{{indent}}{{ module_name }} {%
	 *	if paramsMap|length >0 %}#({{
	 *		paramsMap|join(',\n    ' + indent)}}
	 *{{indent}}    ) {%
	 *  endif %}{{ instanceName }}{%
	 *  if portMaps|length >0 %} ({{
	 *      portMaps|join(',\n    ' + indent)}}
	 *{{indent}}    );
	 *{% endif %}
	 */
	indent(str) << module_name << " ";
	if (params.size()) {
		str << "#(";
		indent_cnt++;
		for (auto p : params) {
			auto src = param_map[p];
			throw std::runtime_error(
					"not implemented Verilog2001::tmpl_component_instance param");
		}
		str << std::endl;
		indent_cnt--;
		indent(str);
		str << ") ";
	}
	if (io.size()) {
		str << " (" << std::endl;
		for (auto p : io) {
			auto src = io_map[p];
			throw std::runtime_error(
					"not implemented Verilog2001::tmpl_component_instance IO");
		}
		str << std::endl;
		indent(str);
		str << ")";
	}
	str << ";";
}

void Verilog2001::serialize_block(const vector<Statement*> & stms,
		std::ostream & str) {
	if (stms.size() == 1) {
		str << std::endl;
		indent_cnt++;
		Serializer::serialize(*stms[0], str);
		indent_cnt--;
	} else {
		str << " begin" << std::endl;

		indent_cnt++;
		for (auto s : stms) {
			Serializer::serialize(*s, str);
		}
		indent_cnt--;

		indent(str) << "end";
	}
}

void Verilog2001::serialize(const Assignment & a, std::ostream & str) {
	assert(a.dst.val == nullptr);

	indent(str);
	// resolve if should use procedural assignment
	auto ver_sig_t = verilogTypeOfSig(a.dst);
	if (ver_sig_t == VERILOG_NET_TYPE::VERILOG_WIRE and a.parent == nullptr)
		str << "assign ";

	// resolve the destination
	serialize_net_usage(a.dst, str);
	for (size_t i = a.dst_index.size(); i > 0; i--) {
		str << "[";
		serialize_net_usage(*a.dst_index[i], str);
		str << "]";
	}

	// resolve symbol of assignment
	if (ver_sig_t == VERILOG_NET_TYPE::VERILOG_REG) {
		bool evDep = false;
		for (auto d : a.dst.drivers) {
			auto driver = dynamic_cast<Statement*>(d);
			// results of operators etc. should not be declared as hidden
			// ant thus all drivers should be only assignments
			if (driver->sens.now_is_event_dependent) {
				evDep = true;
				break;
			}
		}

		if (not evDep) // or _dst.virtualOnly
			str << "=";
		else
			str << "<=";
	} else if (ver_sig_t == VERILOG_NET_TYPE::VERILOG_WIRE) {
		str << "=";
	} else {
		throw std::runtime_error(
				"can not determine type of the Verilog signal");
	}
	serialize_net_usage(a.src, str);
	str << ";";
}

void Verilog2001::serialize(const IfStatement & stm, std::ostream & str) {
	/* {{indent}}if({{ cond }}){%
	 * if ifTrue|length >0 %} begin
	 * {%    for s in ifTrue %}{{s}}
	 * {%
	 *     endfor%}{{indent}}end{%
	 * endif %}{%
	 * if elIfs|length >0 %}{%
	 *     for c, stms in elIfs
	 *        %} else if({{c}}) begin
	 * {%     for s in stms %}{{s}}
	 * {%     endfor%}{{indent}}end{%
	 *     endfor%}{%
	 * endif %}{%
	 * if ifFalse|length >0 %} else begin
	 * {%  for s in ifFalse %}{{s}}
	 * {%  endfor
	 * %}{{indent}}end{%
	 * endif %}
	 */

	indent(str) << "if (";
	serialize(*stm.condition, str);
	str << ")";
	if (stm.ifTrue.size() > 0) {
		serialize_block(stm.ifTrue, str);
	}
	if (stm.elseIf.size() > 0) {
		for (auto & elif : stm.elseIf) {
			str << " else if (";
			serialize_net_usage(*elif.first, str);
			str << ")";
			serialize_block(elif.second, str);
		}
	}

	if (stm.ifFalse.size() > 0) {
		indent(str) << "else";
		serialize_block(stm.ifFalse, str);
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
	if (n.direction != Direction::DIR_UNKNOWN) {
		// is port
		return VERILOG_NET_TYPE::VERILOG_WIRE;
	} else if (driver_cnt == 1) {
		auto d = dynamic_cast<Assignment*>(n.drivers[0]);
		if (d and d->parent and not d->dst_index.size()
				and not d->sens.now_is_event_dependent and not d->src.id.hidden)
			// primitive assignment
			return VERILOG_NET_TYPE::VERILOG_WIRE;
	}

	return VERILOG_NET_TYPE::VERILOG_REG;
}

}
}
