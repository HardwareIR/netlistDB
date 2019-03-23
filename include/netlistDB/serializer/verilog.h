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
			std::function<void(Verilog2001&, const FunctionCall &)>> _specialOp;

	/*
	 * @param io the io which drives where the output will be serialized
	 * @param reserved an additional map of reserved names which can not be used
	 * 		  in target Verilog code
	 * @param delete_io_after if true the io is deleted in destructor of this object
	 */
	Verilog2001(iSerializationIO & io,
			std::map<const std::string, const void*> reserved_names = { },
			bool delete_io_after = false);
	/*
	 * @param str the stream where the output will be serialized
	 * */
	Verilog2001(std::ostream & str,
			std::map<const std::string, const void*> reserved_names = { });
	/*
	 * @param root_dir the output dir where the folder with the output files will be placed
	 * */
	Verilog2001(const std::string & root_dir,
			std::map<const std::string, const void*> reserved_names = { });

	using Serializer::serialize;
	using Serializer::serialize_stm;
	using Serializer::serialize_value;

	virtual bool serialize_type_usage(const hw_type::iHwType & t);
	virtual void serialize_value(
			const typename hw_type::HwInt::value_type & val) override;
	virtual void serialize_net_usage(const Net & n) override;
	virtual void serialize_net_def(const Net & n) override;
	virtual void serialize(const FunctionCall & fncall) override;
	virtual void serialize_io(const Net & io_net) override;

	virtual void serialize_block(const std::vector<Statement*> & stms);

	virtual void serialize_stm(const Assignment & stm) override;
	virtual void serialize_stm(const IfStatement & stm) override;
	virtual void serialize_stm(const HwProcess & stm) override;

	/*
	 * @attention the namescope stays inside of module
	 * */
	virtual void serialize_module_head(const Netlist & netlist) override;
	virtual void serialize_module_body(const Netlist & netlist,
			const std::vector<const HwProcess*> & processes,
			const std::vector<const ComponentMap*> & components) override;
	virtual void serialize(const Netlist & netlist) override;

	virtual void serialize_component_instances(
			const std::vector<const ComponentMap*> & comps);
	virtual void serialize_component_instance(const std::string & module_name,
			const std::string & instance_name,
			const std::map<Net*, Net*> & param_map,
			const std::map<Net*, Net*> & io_map) override;

	void serialize_generic_binOp(const std::string & op_str,
			const FunctionCall & o);
	void serialize_generic_unOp(const std::string & op_str,
			const FunctionCall & o);

	virtual void serialize_operand(const Net & _operand,
			const FunctionCall & oper, bool expr_requires_brackets,
			bool cancel_brances) override;

	std::ostream & indent();
	enum VERILOG_NET_TYPE verilogTypeOfSig(const Net & n) const;
	virtual const std::map<const FunctionDef*, int> & get_operator_precedence()
			override;
	void serialize_tmp_vars();
	void serialize_direction(Direction d);
	void serialize_sensitivity_list_item(const iNode & item,
			bool anyIsEventDependent, const HwProcess & proc);

	const hw_type::iHwType & get_non_array_t(const hw_type::iHwType & _t);
	void print_array_indexes(const hw_type::iHwType * t, bool first);
	virtual ~Verilog2001();
};

}
}
