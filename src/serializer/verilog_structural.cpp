#include <netlistDB/serializer/verilog.h>
#include <algorithm>

using namespace std;

namespace netlistDB {
namespace serializer {

void Verilog2001::serialize_direction(Direction d, std::ostream & str) {
	switch (d) {
	case Direction::DIR_IN:
		str << "input";
		return;
	case Direction::DIR_OUT:
		str << "output";
		return;
	case Direction::DIR_INOUT:
		str << "inout";
		return;
	default:
		throw runtime_error("Invalid value for direction");
	}
}

void Verilog2001::serialize_io(const Net & io_net, std::ostream & str) {
	indent(str);
	serialize_direction(io_net.direction, str);

	VERILOG_NET_TYPE verilog_t;
	if (io_net.direction == Direction::DIR_IN
			or io_net.direction == Direction::DIR_INOUT) {
		verilog_t = VERILOG_NET_TYPE::VERILOG_WIRE;
	} else {
		verilog_t = verilogTypeOfSig(io_net);
	}

	if (verilog_t == VERILOG_NET_TYPE::VERILOG_REG) {
		str << " reg";
	}
	str << " ";
	// extract the non array type from type

	if (serialize_type_usage(get_non_array_t(io_net.t), str)) {
		str << " ";
	}
	str << name_scope.checkedName(io_net.id.name, &io_net);
	// add array sizes if required
	print_array_indexes(&io_net.t, true, str);
}

void Verilog2001::serialize_module_head(const Netlist & netlist,
		ostream & str) {
	//{{indent}}module {{ name }}{% if generics|length >0 %} #({{
	//                     generics|join(',\n'+ indent + '        ')}}
	//{{indent}}    ){% endif %}{% if ports|length >0 %}({{
	//                     ports|join(',\n' + indent + '        ')}}
	//{{indent}}    );{% endif %}

	indent(str) << "module "
			<< name_scope.checkedName(netlist.name, &netlist, true);
	// [TODO] parameters if present

	std::vector<Net*> io;
	for (auto n : netlist.nets) {
		if (n->direction != Direction::DIR_UNKNOWN) {
			io.push_back(n);
		}
	}

	sort(io.begin(), io.end(),
			[this](const Net * a, const Net * b) {
				return name_scope.checkedName(a->id.name, a) < name_scope.checkedName(b->id.name, b);
			});
	if (io.size() > 0) {
		indent_cnt++;
		str << "(" << endl;
		Net* last = io[io.size() - 1];
		for (Net * p : io) {
			serialize_io(*p, str);
			if (p != last) {
				str << "," << endl;
			}
		}
		str << ")";
		indent_cnt--;
	}
	str << ";";
}

void Verilog2001::serialize_module_body(const Netlist & netlist,
		ostream & str) {
	//{% for t in extraTypes %}{{t}};
	//{% endfor %}{% for v in variables %}{{v}};
	//{% endfor %}{% for c in componentInstances %}{{c}}
	//{% endfor %}{% for p in processes %}{{p}}
	//{% endfor %}{{indent}}endmodule
	indent_cnt++;
	bool any_net_def = false;
	for (auto n : netlist.nets) {
		if (not n->id.hidden and n->direction == Direction::DIR_UNKNOWN) {
			serialize_net_def(*n, str);
			str << endl;
			any_net_def = true;
		}
	}
	if (any_net_def)
		str << endl;

	vector<const HwProcess*> processes;
	vector<const ComponentMap*> components;
	for (auto n : netlist.nodes) {
		auto stm = dynamic_cast<Statement*>(n);
		if (stm) {
			auto p = dynamic_cast<HwProcess*>(stm);
			if (p) {
				processes.push_back(p);
			} else {
				throw runtime_error(
						"All statements in netlist should be already wrapped in HwProcess");
			}
			continue;
		}
		auto c = dynamic_cast<ComponentMap*>(n);
		if (c)
			components.push_back(c);
	}
	serialize_component_instances(components, str);
	if (components.size())
		str << endl << endl;

	sort(processes.begin(), processes.end(),
			[this](const HwProcess * a, const HwProcess * b) {
				// [TODO] proper name allocation for the processes with same name
				return name_scope.checkedName(a->name, a) < name_scope.checkedName(b->name, b);
			});

	for (auto p : processes) {
		serialize_stm(*p, str);
		str << endl;
	}
	indent_cnt--;
	str << endl;
	indent(str) << "endmodule";
}

void Verilog2001::serialize(const Netlist & netlist, ostream & str) {
	serialize_module_head(netlist, str);
	str << endl;
	str << endl;
	serialize_module_body(netlist, str);
}

void Verilog2001::serialize_component_instances(
		std::vector<const ComponentMap*> comps, std::ostream & str) {
	sort(comps.begin(), comps.end(),
			[this](const ComponentMap * a, const ComponentMap * b) {
				// [TODO] proper name allocation for the components with same name
				return name_scope.checkedName(a->component->name, a) < name_scope.checkedName(b->component->name, b);
			});

	for (auto c : comps) {
		auto c_name = name_scope.checkedName(c->component->name, &c->component,
				true);
		auto inst_name = name_scope.checkedName(c->component->name, c);
		std::map<Net*, Net*> param_map;
		serialize_component_instance(c_name, inst_name, param_map,
				c->child_to_parent, str);
	}
}

void Verilog2001::serialize_component_instance(const std::string & module_name,
		const std::string & instance_name,
		const std::map<Net*, Net*> & param_map,
		const std::map<Net*, Net*> & io_map, std::ostream & str) {
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
	if (param_map.size()) {
		str << "#(";
		indent_cnt++;
		for (auto p : param_map) {
			// auto src = param_map[p];
			throw std::runtime_error(
					"not implemented Verilog2001::serialize_component_instance param");
		}
		str << std::endl;
		indent_cnt--;
		indent(str);
		str << ") ";
	}
	if (io_map.size()) {
		str << "(" << std::endl;

		indent_cnt++;
		for (auto iter = io_map.begin(); iter != io_map.end(); ) {
			auto port = iter->first;
			auto par_sig = iter->second;
		  indent(str) << "." << name_scope.checkedName(port->id.name, port);
		  str << "(";
		  serialize_net_usage(*par_sig, str);
		  str << ")";

		  if (++iter != io_map.end())
		      str << "," << endl;
		}
		str << std::endl;
		indent_cnt--;
		indent(str);
		str << ")";
	}
	str << ";";
}

}
}
