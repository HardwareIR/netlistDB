#include <netlistDB/serializer/verilog.h>

using namespace std;

namespace netlistDB {
namespace serializer {

void Verilog2001::serialize_stm(const Assignment & a) {
	assert(a.dst.val == nullptr);
	auto & str = io.str();

	indent();
	// resolve if should use procedural assignment
	auto ver_sig_t = verilogTypeOfSig(a.dst);
	if (ver_sig_t == VERILOG_NET_TYPE::VERILOG_WIRE and a.parent == nullptr)
		str << "assign ";

	// resolve the destination
	serialize_net_usage(a.dst);
	for (size_t i = a.dst_index.size(); i > 0; i--) {
		str << "[";
		serialize_net_usage(*a.dst_index[i - 1]);
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
	serialize_net_usage(a.src);
	str << ";";
}

void Verilog2001::serialize_stm(const IfStatement & stm) {
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
	auto & str = io.str();
	if (tmp_extractor.is_unused(*stm.condition)) {
		assert(stm.elseIf.size() == 0);
		assert(stm.ifFalse.size() == 0);
		assert(stm.ifFalse_specified == false);
		auto last = stm.ifTrue.back();
		for (auto s : stm.ifTrue) {
			serialize_stm(*s);
			if (s != last) {
				str << endl;
			}
		}
		return;
	}

	indent() << "if (";
	serialize_net_usage(*stm.condition);
	str << ")";
	if (stm.ifTrue.size() > 0) {
		serialize_block(stm.ifTrue);
	}
	bool last_is_begin_end_block = stm.ifTrue.size() != 1;
	if (stm.elseIf.size() > 0) {
		for (auto & elif : stm.elseIf) {
			if (last_is_begin_end_block)
				str << " ";
			else
				indent();

			str << "else";
			if (tmp_extractor.is_unused(*elif.first)) {
				assert(&stm.elseIf.back() == &elif);
				assert(stm.ifFalse.size() == 0);
				assert(stm.ifFalse_specified == false);
				serialize_block(elif.second);
				return;
			}
			str << " if (";
			serialize_net_usage(*elif.first);
			str << ")";
			serialize_block(elif.second);

			last_is_begin_end_block = elif.second.size() != 1;
		}
	}

	if (stm.ifFalse.size() > 0) {
		if (last_is_begin_end_block)
			str << " ";
		else
			indent();
		str << "else";
		serialize_block(stm.ifFalse);
	}
}
void Verilog2001::serialize_tmp_vars() {
	for (auto & v : tmp_extractor.replacements) {
		if (v.second == tmp_extractor.unused)
			continue;
		throw std::runtime_error(
				std::string(__FILE__) + ":" + std::to_string(__LINE__) + ": Not implemented");
	}
}

void Verilog2001::serialize_stm(const HwProcess & proc) {
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
	auto & str = io.str();
	if (hasToBeProcess) {
		indent();
		if (proc.sensitivityList.size()) {
			str << "always @(";
			const iNode* last = proc.sensitivityList.back();
			for (const iNode* si : proc.sensitivityList) {
				serialize_sensitivity_list_item(*si, anyIsEventDependnt, proc);
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
		serialize_tmp_vars();
		for (auto s : proc.statements) {
			serialize_stm(*s);
			str << endl;
		}
		indent_cnt--;
		indent() << "end";
	} else {
		auto last = proc.statements.back();
		for (auto stm: proc.statements) {
			serialize_stm(*stm);
			if (stm != last)
				str << endl;
		}

	}
	tmp_extractor.clear();
}

}
}
