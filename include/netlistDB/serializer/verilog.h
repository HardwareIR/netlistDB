#pragma once

#include <iostream>
#include <set>
#include <map>
#include <functional>
#include <vector>

#include <netlistDB/netlist.h>
#include <netlistDB/serializer/serializer.h>
#include <netlistDB/serializer/namescope.h>
#include <netlistDB/serializer/verilog_tmp_var_extractor.h>

namespace netlistDB {
namespace serializer {

/*
 * Convert Netlist instance to Verilog string
 * @note see doc on Serializer class
 * */
class Verilog2001: public Serializer {
public:
	NameScope name_scope;
	VerilogTmpVarExtractor tmp_extractor;
	size_t indent_cnt;

	enum VERILOG_NET_TYPE {
		VERILOG_WIRE, VERILOG_REG,
	};

	static const std::vector<std::string> keywords;
	static const std::string DEFAULT_FILE_EXT;
	static const std::string INDENT;
	static const std::map<const FunctionDef*, int> opPrecedence;
	// map of the string for unary operators
	static const std::map<const FunctionDef*, const std::string> _unaryOps;
	// map of the strings for binary operators
	static const std::map<const FunctionDef*, const std::string> _binOps;
	// map of the serialization functions for operators which special syntax
	static const std::map<const FunctionDef*,
			std::function<
					void(Verilog2001&, const FunctionCall &, std::ostream & str)>> _specialOp;

	/*
	 * @param reserved an additional map of reserved names which can not be used
	 * 		  in target Verilog code
	 */
	Verilog2001(std::map<const std::string, const void*> reserved_names = { });

	using Serializer::serialize;
	using Serializer::serialize_stm;
	using Serializer::serialize_value;

	virtual bool serialize_type_usage(const hw_type::iHwType & t,
			std::ostream & str) override {
		return false;
	}
	virtual void serialize_value(const typename hw_type::HwInt::value_type & val, std::ostream & str) override;
	virtual void serialize_net_usage(const Net & n, std::ostream & str)
			override;
	virtual void serialize_net_def(const Net & n, std::ostream & str) override {
		throw std::runtime_error(
				std::string(__PRETTY_FUNCTION__) + "not implemented");
	}
	virtual void serialize(const FunctionCall & fncall, std::ostream & str)
			override;
	virtual void serialize_io(const Net & io_net, std::ostream & str) override;

	virtual void serialize_block(const std::vector<Statement*> & stms,
			std::ostream & str);

	virtual void serialize_stm(const Assignment & stm, std::ostream & str) override;
	virtual void serialize_stm(const IfStatement & stm, std::ostream & str)
			override;
	virtual void serialize_stm(const HwProcess & stm, std::ostream & str) override;

	virtual void serialize_module_head(const Netlist & netlist,
			std::ostream & str) override;
	virtual void serialize_module_body(const Netlist & netlist,
			std::ostream & str) override;
	virtual void serialize(const Netlist & netlist, std::ostream & str)
			override;

	virtual void serialize_component_instance(const std::string & module_name,
			const std::string & instance_name, std::vector<Net*> & params,
			std::map<Net*, Net*> param_map, std::vector<Net*> & io,
			std::map<Net*, Net*> io_map, std::ostream & str) override;

	void serialize_generic_binOp(const std::string & op_str,
			const FunctionCall & o, std::ostream & str);
	void serialize_generic_unOp(const std::string & op_str,
			const FunctionCall & o, std::ostream & str);

	virtual void serialize_operand(const Net & _operand,
			const FunctionCall & oper, bool expr_requires_braces,
			std::ostream & str) override;

	std::ostream & indent(std::ostream & str);
	enum VERILOG_NET_TYPE verilogTypeOfSig(const Net & n) const;
	virtual const std::map<const FunctionDef*, int> & get_operator_precedence()
			override;
	void serialize_tmp_vars(std::ostream & str);
	void serialize_direction(Direction d, std::ostream & str);
	void serialize_sensitivity_list_item(const iNode & item,
			bool anyIsEventDependent, const HwProcess & proc,
			std::ostream & str);
	virtual ~Verilog2001();
};

}
}
