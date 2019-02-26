#include <netlistDB/serializer/verilog.h>

using namespace std;

namespace netlistDB {
namespace serializer {

Verilog2001::Verilog2001(
		std::map<const std::string &, const void*> reserved_names) :
		name_scope(true) {
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
	str << INDENT << module_name << " ";
	if (params.size()) {
		str << "#(";
		for (auto p : params) {
			auto src = param_map[p];
			throw std::runtime_error(
					"not implemented Verilog2001::tmpl_component_instance param");
		}
		str << ") ";
	}
	if (io.size()) {
		str << " (" << std::endl;
		for (auto p : io) {
			auto src = io_map[p];
			throw std::runtime_error(
					"not implemented Verilog2001::tmpl_component_instance IO");
		}
		str << ")";
	}
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

	str << INDENT << "if (";
	serialize(*stm.condition, str);
	str << ")";
	if (stm.ifTrue.size() > 0) {
		str << " begin" << std::endl;
		for (auto s : stm.ifTrue) {

		}
	}
}

void Verilog2001::indent(size_t cnt, std::ostream & str) {
//	for (size_t i = 0; i < cnt; i++) {
//		str << INDENT;
//	}
}


}
}
