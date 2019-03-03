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
	if (serialize_type_usage(io_net.t, str)) {
		str << " ";
	}
	str << name_scope.checkedName(io_net.id.name, &io_net);
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
	vector<HwProcess*> processes;
	for (auto n: netlist.nodes) {
		auto stm = dynamic_cast<Statement*>(n);
		if (stm) {
			auto p = dynamic_cast<HwProcess*>(stm);
			if (p) {
				processes.push_back(p);
			} else {
				throw runtime_error("All statements in netlist should be already wrapped in HwProcess");
			}
		}
	}
	sort(processes.begin(), processes.end(),
			[this](const HwProcess * a, const HwProcess * b) {
				// [TODO] proper name allocation for the processes with same name
				return name_scope.checkedName(a->name, a) < name_scope.checkedName(b->name, b);
			});
	indent_cnt++;
	for (auto p: processes) {
		serialize(*p, str);
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
			// auto src = param_map[p];
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

		indent_cnt++;
		for (auto p : io) {
			// auto src = io_map[p];
			throw std::runtime_error(
					"not implemented Verilog2001::tmpl_component_instance IO");
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
