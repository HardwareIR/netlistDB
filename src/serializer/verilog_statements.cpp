#include <netlistDB/serializer/verilog.h>

namespace netlistDB {
namespace serializer {

void Verilog2001::serialize(const Statement & stm, std::ostream & str) {
	Serializer::serialize(stm, str);
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
			str << " = ";
		else
			str << " <= ";
	} else if (ver_sig_t == VERILOG_NET_TYPE::VERILOG_WIRE) {
		str << " = ";
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

}
}
