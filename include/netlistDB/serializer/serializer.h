#pragma once

#include <netlistDB/netlist.h>
#include <netlistDB/statement_assignment.h>
#include <netlistDB/statement_if.h>
#include <netlistDB/statement_hwprocess.h>
#include <netlistDB/hw_type/hw_int.h>

namespace netlistDB {
namespace serializer {

/*
 * The Serializers are converting the Netlist instance to target HDL or other format
 * The serialization process is not destructive but it can modify the netlist.
 * The names may be changed to avoid the collision with language keyword etc.
 * Some expressions can be rewritten as the target format does not support them.
 * This results in order dependency if multiple serializerations are applied.
 * In order to avoid  this use different copy Netlist.
 * */
class Serializer {
public:

	/*
	 * Serialize the type id
	 *
	 * @return true if there was something added to str
	 * */
	virtual bool serialize_type_usage(const hw_type::iHwType & t,
			std::ostream & str) = 0;
	/*
	 * Serialize the value of specified type
	 * */
	virtual void serialize_value(const hw_type::iHwTypeValue & val, std::ostream & str);
	virtual void serialize_value(const typename hw_type::HwInt::value_type & val, std::ostream & str) = 0;

	/*
	 * Serialize the id of the net in expression
	 * */
	virtual void serialize_net_usage(const Net & n, std::ostream & str) = 0;
	/*
	 * Serialize a definition of internal signal in module
	 * */
	virtual void serialize_net_def(const Net & n, std::ostream & str) = 0;
	/*
	 * Serialize calls to functions and operators to target HDL
	 * */
	virtual void serialize(const FunctionCall & fncall, std::ostream & str) = 0;
	/*
	 * Serialize an IO (port) definition for module header
	 * */
	virtual void serialize_io(const Net & io_net, std::ostream & str) = 0;
	/*
	 * Proxy for serialization methods of all statements
	 * */
	virtual void serialize_stm(const Statement & stm, std::ostream & str);
	/*
	 * Serialize assignment statement to target HDL
	 * */
	virtual void serialize_stm(const Assignment & stm, std::ostream & str) = 0;
	/*
	 * Serialize if statement to target HDL
	 **/
	virtual void serialize_stm(const IfStatement & stm, std::ostream & str) = 0;
	/*
	 * Serialize if statement to target HDL
	 **/
	virtual void serialize_stm(const HwProcess & stm, std::ostream & str) = 0;
	/*
	 * Serialize module header with IO specification to target HDL
	 * */
	virtual void serialize_module_head(const Netlist & netlist,
			std::ostream & str) = 0;
	/*
	 * Serialize the internal signals definitions, components and processes to target HDL
	 * */
	virtual void serialize_module_body(const Netlist & netlist,
			std::ostream & str) = 0;

	/*
	 * Serialize whole context as target HDL module recursively
	 * */
	virtual void serialize(const Netlist & netlist, std::ostream & str) = 0;

	/*
	 * Template for component instance
	 * */
	virtual void serialize_component_instance(const std::string & module_name,
			const std::string & instance_name, std::vector<Net*> & params,
			std::map<Net*, Net*> param_map, std::vector<Net*> & io,
			std::map<Net*, Net*> io_map, std::ostream & str) = 0;

	/*
	 * Serialize the operand, optionally wrap in to braces
	 *
	 * @param expr_requires_braces if true and the operand is any form of expression
	 * 			the braces will be added
	 * */
	virtual void serialize_operand(const Net & operand,
			const FunctionCall & oper, bool expr_requires_braces,
			std::ostream & str);

	virtual const std::map<const FunctionDef*, int> & get_operator_precedence() = 0;

	virtual ~Serializer() {
	}
};

}

}
