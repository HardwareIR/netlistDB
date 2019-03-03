#pragma once

#include <set>
#include <map>

#include <netlistDB/query/statement_visitor.h>

namespace netlistDB {
namespace serializer {

class VerilogTmpVarExtractor: public query::StatementVisitor {
protected:
	Netlist replacement_ctx;
	bool operand_is_another_operand(const Net & operand);
	void extract_as_tmp_var(const Net & n, const std::string & name_prefix);

public:
	// original -> replacement (tmp variable net)
	std::map<const Net *, const Net*> replacements;

	VerilogTmpVarExtractor();

	/*
	 * Clear all replacement info containers
	 * */
	void clear();
	/*
	 * @return original net or replacement if net was replaced
	 * */
	const Net & checked(const Net & n);

	using query::StatementVisitor::visit;
	virtual void visit(const FunctionCall & c) override;
};

}
}
