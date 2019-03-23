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

	name_scope.level_push();
	std::vector<const Net*> io;
	for (auto n : netlist.nets) {
		if (n->direction != Direction::DIR_UNKNOWN) {
			io.push_back(n);
		}
	}
	sort_named_objects<const Net>(io, [](const Net* n) {
		return n->id.name;
	});

	if (io.size() > 0) {
		indent_cnt++;
		str << "(" << endl;
		const Net* last = io[io.size() - 1];
		for (const Net * p : io) {
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

void get_proc_and_comp(const Netlist & netlist,
		vector<const HwProcess*> & processes,
		vector<const ComponentMap*> & components) {
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
}

void Verilog2001::serialize_module_body(const Netlist & netlist,
		const vector<const HwProcess*> & processes,
		const vector<const ComponentMap*> & components, ostream & str) {
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

	serialize_component_instances(components, str);
	if (components.size())
		str << endl << endl;

	for (auto p : processes) {
		serialize_stm(*p, str);
		str << endl;
	}
	indent_cnt--;
	str << endl;
	indent(str) << "endmodule";
}

void Verilog2001::serialize(const Netlist & netlist, ostream & str) {
	vector<const HwProcess*> processes;
	vector<const ComponentMap*> components;
	get_proc_and_comp(netlist, processes, components);
	sort_named_objects<const ComponentMap>(components,
			[](const ComponentMap* c) {return c->component->name;});
	sort_named_objects<const HwProcess>(processes,
			[](const HwProcess* p) {return p->name;});
	// serialize all dependencies
	for (auto comp : components) {
		if (name_scope.all_names.find(&comp->component)
				== name_scope.all_names.end()) {
			// if it does not have name the definition of the component module has to be serialized first
			// Otherwise it is not required to serialize module definition because it already happened
			serialize(*comp->component, str);
			str << endl;
			str << endl;
			str << endl;
		}
	}

	serialize_module_head(netlist, str);
	str << endl;
	str << endl;
	serialize_module_body(netlist, processes, components, str);
	name_scope.level_pop();
}

void Verilog2001::serialize_component_instances(
		const std::vector<const ComponentMap*> & comps, std::ostream & str) {
	for (auto c : comps) {
		auto c_name = name_scope.checkedName(c->component->name, c->component.get(),
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
		if (param_map.size()) {
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
		vector<Net*> ports;
		for (auto m : io_map) {
			ports.push_back(m.first);
		}
		sort_named_objects<Net>(ports, [](Net* n) {return n->id.name;});

		for (Net * port : ports) {
			auto par_sig = io_map.find(port)->second;
			indent(str) << "." << name_scope.checkedName(port->id.name, port);
			str << "(";
			serialize_net_usage(*par_sig, str);
			str << ")";

			if (port != ports.back())
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
