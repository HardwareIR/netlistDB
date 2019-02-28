#pragma once

#include <iostream>
#include <set>
#include <map>
#include <functional>
#include <vector>

#include <netlistDB/netlist.h>
#include <netlistDB/serializer/serializer.h>
#include <netlistDB/serializer/namescope.h>


namespace netlistDB {
namespace serializer {

/*
 * Convert Netlist instance to Verilog string
 * @note see doc on Serializer class
 * */
class Verilog2001: public Serializer {
public:
	NameScope name_scope;
	size_t indent_cnt;

	enum VERILOG_NET_TYPE {
		VERILOG_WIRE,
		VERILOG_REG,
		//VERILOG_PORT,
	};

	static const std::vector<std::string> keywords;
	static const std::string DEFAULT_FILE_EXT;
	static const std::string INDENT;
	static const std::map<const FunctionDef*, int> opPrecedence;
	static const std::map<const FunctionDef*,
			std::function<void(const std::string&, std::ostream &)>> _unaryOps;
	static const std::map<const FunctionDef*,
			std::function<
					void(const std::string&, const std::string&, std::ostream &)>> _binOps;
	/*
	 * @param reserved an additional map of reserved names which can not be used
	 * 		  in target verilog code
	 */
	Verilog2001(std::map<const std::string, const void*> reserved_names = { });

	virtual void serialize_type_usage(const hw_type::iHwType & t,
			std::ostream & str) override {};
	virtual void serialize_net_usage(const Net & n, std::ostream & str)
			override {};
	virtual void serialize_net_def(const Net & n, std::ostream & str) override {};
	virtual void serialize(const FunctionCall & fncall, std::ostream & str)
			override {};
	virtual void serialize_io(const Net & io_net, std::ostream & str) override {};
	virtual void serialize(const Statement & stm, std::ostream & str) override {};
	virtual void serialize_block(const std::vector<Statement*> & stms, std::ostream & str);
	virtual void serialize(const IfStatement & stm, std::ostream & str)
			override;
	virtual void serialize(const Assignment & stm, std::ostream & str) override;
	virtual void serialize_module_head(const Netlist & netlist,
			std::ostream & str) override;
	virtual void serialize_module_body(const Netlist & netlist,
			std::ostream & str) override;
	virtual void serialize(const Netlist & netlist, std::ostream & str)
			override;

	virtual void serialize_component_instance(const std::string & module_name,
			const std::string & instance_name,
			std::vector<Net*> & params, std::map<Net*, Net*> param_map,
			std::vector<Net*> & io, std::map<Net*, Net*> io_map,
			std::ostream & str) override;

	std::ostream & indent(std::ostream & str);
	enum VERILOG_NET_TYPE verilogTypeOfSig(const Net & n) const;

	virtual ~Verilog2001();
};

}
}
