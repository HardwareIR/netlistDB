#include <netlistDB/serializer/verilog.h>

using namespace std;

namespace netlistDB {
namespace serializer {

void Verilog2001::serialize(const Assignment & a, ostream & str) {
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
		throw runtime_error("can not determine type of the Verilog signal");
	}
	serialize_net_usage(a.src, str);
	str << ";";
}

void Verilog2001::serialize(const IfStatement & stm, ostream & str) {
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
	if (tmp_extractor.is_unused(*stm.condition)) {
		assert(stm.elseIf.size() == 0);
		assert(stm.ifFalse.size() == 0);
		assert(stm.ifFalse_specified == false);
		auto last = stm.ifTrue.back();
		for (auto s : stm.ifTrue) {
			Serializer::serialize(*s, str);
			if (s != last) {
				str << endl;
			}
		}
		return;
	}

	indent(str) << "if (";
	serialize_net_usage(*stm.condition, str);
	str << ")";
	if (stm.ifTrue.size() > 0) {
		serialize_block(stm.ifTrue, str);
	}
	bool last_is_begin_end_block = stm.ifTrue.size() != 1;
	if (stm.elseIf.size() > 0) {
		for (auto & elif : stm.elseIf) {
			if (last_is_begin_end_block)
				str << " ";
			else
				indent(str);

			str << "else";
			if (tmp_extractor.is_unused(*elif.first)) {
				assert(&stm.elseIf.back() == &elif);
				assert(stm.ifFalse.size() == 0);
				assert(stm.ifFalse_specified == false);
				serialize_block(elif.second, str);
				return;
			}
			str << " if (";
			serialize_net_usage(*elif.first, str);
			str << ")";
			serialize_block(elif.second, str);

			last_is_begin_end_block = elif.second.size() != 1;
		}
	}

	if (stm.ifFalse.size() > 0) {
		if (last_is_begin_end_block)
			str << " ";
		else
			indent(str);
		str << "else";
		serialize_block(stm.ifFalse, str);
	}
}
void Verilog2001::serialize_tmp_vars(ostream & str) {
	for (auto & v : tmp_extractor.replacements) {
		if (v.second == tmp_extractor.unused)
			continue;
		throw std::runtime_error(
				string(__PRETTY_FUNCTION__) + ": Not implemented");
	}
}

void Verilog2001::serialize(const HwProcess & proc, ostream & str) {
	// {% if hasToBeProcess
	//    %}{{indent}}{%
	//     if sensitivityList|length>0
	//          %}always @({{ sensitivityList}}){%
	//     else %}always_comb
	// {%  endif%} begin: {{ name }}{%
	//     if extraVars %}{%
	//         for ev in extraVars%}
	// {{           indent}}{{ev}};{%      endfor%}{%
	//     endif %}{%
	//     for s in statements %}
	// {{       s}}{%
	//     endfor %}
	// {{  indent}}end
	// {% else %}{%
	//     if extraVars %}{%
	//         for ev in extraVars%}
	// {{            ev}};
	// {%      endfor%}{%
	//     endif %}{%
	//     for s in statements
	//        %}{{s}}{%
	//     endfor %}{%
	//  endif %}
	tmp_extractor.visit(proc);

	bool anyIsEventDependnt = false;
	for (auto s : proc.sensitivityList) {
		if (dynamic_cast<const FunctionCall*>(s)) {
			anyIsEventDependnt = true;
			break;
		}
	}

	bool hasToBeProcess = anyIsEventDependnt or tmp_extractor.replacements.size() > 0;
	if (not hasToBeProcess) {
		for (auto o : proc.outputs)
			if (verilogTypeOfSig(*o) == VERILOG_NET_TYPE::VERILOG_REG) {
				hasToBeProcess = true;
				break;
			}
	}
	if (hasToBeProcess) {
		indent(str);
		if (proc.sensitivityList.size()) {
			str << "always @(";
			const iNode* last = proc.sensitivityList.back();
			for (const iNode* si : proc.sensitivityList) {
				serialize_sensitivity_list_item(*si, anyIsEventDependnt, proc, str);
				if (si != last) {
					str << " or ";
				}
			}
			str << ")";
		} else {
			str << "always_comb";
		}
		str << " begin: " << name_scope.checkedName(proc.name, &proc) << endl;
		indent_cnt++;
		serialize_tmp_vars(str);
		for (auto s : proc.statements) {
			serialize(*s, str);
			str << endl;
		}
		indent_cnt--;
		indent(str) << "end";
	} else {
		auto last = proc.statements.back();
		for (auto stm: proc.statements) {
			serialize(*stm, str);
			if (stm != last)
				str << endl;
		}

	}
	tmp_extractor.clear();
}

}
}
