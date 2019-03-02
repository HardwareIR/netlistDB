#include <netlistDB/serializer/verilog.h>

using namespace std;

namespace netlistDB {
namespace serializer {

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

}
}
