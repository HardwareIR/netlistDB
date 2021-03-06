#pragma once

#include <netlistDB/netlist.h>
#include <netlistDB/statement_assignment.h>
#include <netlistDB/statement_if.h>
#include <netlistDB/statement_hwprocess.h>
#include <netlistDB/hw_type/hw_int.h>
#include <netlistDB/serializer/serialization_io.h>

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
	bool delete_io_after;
	iSerializationIO & io;
	Serializer(iSerializationIO & io, bool delete_io_after);

	/*
	 * Serialize the comment string
	 *
	 * @note may contain newlines etc.
	 * */
	virtual void serialize_comment(const std::string & comment) = 0;

	/*
	 * Serialize the type id
	 *
	 * @return true if there was something added to str
	 * */
	virtual bool serialize_type_usage(const hw_type::iHwType & t) = 0;
	/*
	 * Serialize the value of specified type
	 * */
	virtual void serialize_value(const hw_type::iHwTypeValue & val);
	virtual void serialize_value(
			const typename hw_type::HwInt::value_type & val) = 0;

	/*
	 * Serialize the id of the net in expression
	 * */
	virtual void serialize_net_usage(const Net & n) = 0;
	/*
	 * Serialize a definition of internal signal in module
	 * */
	virtual void serialize_net_def(const Net & n) = 0;
	/*
	 * Serialize calls to functions and operators to target HDL
	 * */
	virtual void serialize(const FunctionCall & fncall) = 0;
	/*
	 * Serialize an IO (port) definition for module header
	 * */
	virtual void serialize_io(const Net & io_net) = 0;
	/*
	 * Proxy for serialization methods of all statements
	 * */
	virtual void serialize_stm(const Statement & stm);
	/*
	 * Serialize assignment statement to target HDL
	 * */
	virtual void serialize_stm(const Assignment & stm) = 0;
	/*
	 * Serialize if statement to target HDL
	 **/
	virtual void serialize_stm(const IfStatement & stm) = 0;
	/*
	 * Serialize if statement to target HDL
	 **/
	virtual void serialize_stm(const HwProcess & stm) = 0;
	/*
	 * Serialize module header with IO specification to target HDL
	 * */
	virtual void serialize_module_head(const Netlist & netlist) = 0;
	/*
	 * Serialize the internal signals definitions, components and processes to target HDL
	 * */
	virtual void serialize_module_body(const Netlist & netlist,
			const std::vector<const HwProcess*> & processes,
			const std::vector<const ComponentMap*> & components) = 0;

	/*
	 * Serialize whole context as target HDL module recursively
	 * */
	virtual void serialize(const Netlist & netlist) = 0;

	/*
	 * Template for component instance
	 * */
	virtual void serialize_component_instance(const std::string & module_name,
			const std::string & instance_name,
			const std::map<Net*, Net*> & param_map,
			const std::map<Net*, Net*> & io_map) = 0;

	/*
	 * Serialize the operand, optionally wrap in to brackets
	 *
	 * @param expr_requires_brackets if true and the operand is any form of expression
	 * 			the brackets will be always added
	 * 		 (if false the brackets will be added only if required)
	 * @param cancel_brances if true the brackets are never added
	 * */
	virtual void serialize_operand(const Net & operand,
			const FunctionCall & oper, bool expr_requires_brackets,
			bool cancel_brackets);
	int precedence_of_expr(const Net & n);
	virtual const std::map<const FunctionDef*, int> & get_operator_precedence() = 0;

	template<class T>
	void sort_named_objects(std::vector<T*> & objs,
			std::function<const std::string(T * obj)> name_getter) {
		sort(objs.begin(), objs.end(), [&](T * a, T * b) {
			auto a_name = name_getter(a);
			auto b_name = name_getter(b);

			if (a_name == b_name) {
				return a->index < b->index;
			} else {
				return a_name < b_name;
			}
		});
	}

	virtual ~Serializer();
};

}

}
