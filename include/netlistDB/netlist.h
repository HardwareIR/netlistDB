#pragma once

/**
 * This file contains the definitions of the basic building blocks of the netlistDB
 *
 * Main container of the graph is represented by Netlist class.
 * The netlist is composed of Signal instances, where each instance
 * has its endpoints and drivers, endpoint and driver can be Statement of FunctionCall.
 *
 **/

#include <set>
#include <assert.h>
#include <vector>
#include <unordered_map>
#include <atomic>

#include <netlistDB/utils/ordered_set.h>
#include <netlistDB/varId.h>
#include <netlistDB/constants.h>
#include <netlistDB/pointer_container.h>
#include <netlistDB/function_def.h>
#include <netlistDB/utils/chained_iterator.h>
#include <netlistDB/hw_type/ihw_type.h>
#include <netlistDB/hw_type/ihw_type_value.h>

namespace netlistDB {

class Net;
class Netlist;
class Statement;

/**
 * Inteface for nodes in database
 *
 **/
class iNode {
public:
	// sequential number used as a id during serialization
	// and as a index in main list of iNode instances in Netlist.nodes
	size_t index;

	using iterator = utils::ChainedSequence<iNode*>;
	using predicate_t = std::function<bool(iNode*)>;

	// iterator of endpoints for Net or outputs for statement or result for expression
	iterator forward;
	// iterator of drivers for net or inputs for statement or args for expression
	iterator backward;

	virtual ~iNode() {
	}
};

class OperationNode: public iNode {
};

/*
 * Sensitivity list used for resolution of sensitivity for hw statement instances
 *
 * @ivar contains_event_dep: true if this contains event dependent
 *    sensitivity
 */

class SensitivityCtx: public utils::OrderedSet<iNode*> {
public:
	bool contains_event_dep;
	SensitivityCtx() :
			contains_event_dep(false) {
	}

	template<typename iterable>
	void extend(const iterable & other) {
		_extend<iterable>(other);
	}

	void clear() {
		utils::OrderedSet<iNode*>::clear();
		contains_event_dep = false;
	}
private:
	template<typename iterable>
	void _extend(const iterable & other) {
		utils::OrderedSet<iNode*>::extend(other);
	}
	void _extend(const SensitivityCtx & other) {
		utils::OrderedSet<iNode*>::extend(other);
		contains_event_dep |= other.contains_event_dep;
	}

};

/*
 * @ivar _is_completly_event_dependent statement does not have
 *      any combinational or async statement
 * @ivar _is_partially_event_dependent statement depends on clock, but may
 * 		contain async logic
 * @ivar _sensitivity OrderedSet of input signals
 *     or (rising/falling) operator
 * @ivar _enclosed_for set of outputs for which this statement is enclosed
 *     (for which there is not any unused branch = where is not latch)
 *
 * */
class SensitivityInfo {
public:
	bool is_completly_event_dependent;
	bool now_is_event_dependent;
	utils::OrderedSet<Net*> enclosed_for;
	SensitivityCtx sensitivity;
	SensitivityInfo() :
			is_completly_event_dependent(false), now_is_event_dependent(false) {
	}
};

/*
 * @ivar parent parent instance of HdlStatement or nullptr
 * @ivar _inputs OrderedSet of input signals for this statement
 * @ivar _outputs OrderedSet of output signals for this statement

 * @ivar rank sum of numbers of used branches in statement, used as prefilter
 *     for statement comparing
 *
 * @attention the sensitivity has to be discovered explicitly
 */
class Statement: public OperationNode {
public:
	Statement * parent;

	// IO of this statement
	utils::OrderedSet<Net*> _inputs;
	utils::OrderedSet<Net*> _outputs;

	SensitivityInfo sens;

	size_t rank;

	Statement() :
			parent(nullptr), rank(0) {
	}

	/*
	 * Set parent statement and update it's sensitivity and IO
	 * */
	void _set_parent_stm(Statement * stm);
	/*
	 * After parent statement become event dependent
	 * propagate event dependency flag to child statements
	 */
	void _on_parent_event_dependent();

	/*
	 * @return iterator over all (direct) children statements
	 * */
	virtual utils::ChainedSequence<Statement*> _iter_stms() = 0;

	/*
	 * @return the Netlist where signals for connected to this statement are created
	 * */
	Netlist & _get_context();

	/*
	 * call _set_parent_stm for the collection and copy the vector to target
	 * */
	void _register_stements(const std::vector<Statement*> & statements,
			std::vector<Statement*> & target);

};

/**
 * Container of call of the function (operator is also function)
 */
class FunctionCall: public OperationNode {
public:
	// definition of the function
	FunctionDef & fn;
	// arguments specified in call
	std::vector<Net*> args;
	// result of the function, operator result or return from function call
	Net & res;

	FunctionCall(const FunctionCall& other) = delete;
	FunctionCall(FunctionDef & fn, Net & op0, Net & res);
	FunctionCall(FunctionDef & fn, Net & op0, Net & op1, Net & res);
};

/**
 * Hyperedge which connects the the statements, expressions, etc.
 * */
class Net: public iNode {
public:
	// container of name and type
	VarId id;
	// netlist which is the owner of this signal
	Netlist & ctx;

	// the index of this net in Netlist.nets
	size_t net_index;

	// type of value of this signal
	hw_type::iHwType & t;
	// the optional value of this signal
	// if the value is specified the signal is constant
	hw_type::iHwTypeValue * val;
	// the value used if net does not have any explicit driver
	Net * nop_val;
	// the value after reset
	Net * def_val;

	// direction of the signal if signal is used in IO
	Direction direction;
	// operators/ statements which are driving the value of this signal
	utils::OrderedSet<OperationNode*> drivers;
	utils::OrderedSet<OperationNode*> endpoints;
	using UsageCacheKey = _UsageCacheKey<FunctionDef, Net>;
	std::unordered_map<UsageCacheKey, Net*> usage_cache;
	// index used for last priority ordering
	// represent the sequential number of the signal generated in parent context

	Net(const Net & other) = delete;
	// use methods from Netlist
	Net(Netlist & ctx, hw_type::iHwType & t, const std::string & name,
			Direction direction);
	bool is_const();

	Net & operator!() = delete;
	Net & operator~();
	Net & operator|(Net & other);
	Net & operator&(Net & other);
	Net & operator^(Net & other);

	Net & operator<=(Net & other);
	Net & operator<(Net & other);
	Net & operator>=(Net & other);
	Net & operator>(Net & other);
	Net & operator==(Net & other);
	Net & operator!=(Net & other);

	Net & operator-();
	Net & operator+(Net & other);
	Net & operator-(Net & other);
	Net & operator*(Net & other);
	Net & operator/(Net & other);

	Net & operator[](Net & index);
	Net & concat(Net & other);
	Net & rising();
	Net & falling();

	// as assignment
	Statement & operator()(Net & other);

	// disconnect the selected endpoints
	void forward_disconnect(iNode::predicate_t pred);
};

/**
 * Circuit graph representation
 *
 * There are multiple types of node in this directed graph
 * The signal, statement and FunctionCall (which represents also the operator usage)
 *
 * Each node has own list of neighbors. This is the case because the graph
 * is expected to change frequently and be larger than 10k nodes this representation
 * by linked list or connection matrix would not be efficient.
 *
 * Also for some nodes the order of neighbors does matter for some does not.
 * For example order of Signal endpoints does not matter. The order of Signals
 * in FunctionCall does.
 *
 **/
class Netlist {
public:
	// name for debugging purposes
	std::string name;
	std::vector<Net*> nets;
	std::vector<iNode*> nodes;

	Netlist(const Netlist & other) = delete;
	Netlist(const std::string & name);

	// create input signal
	Net & sig_in(const std::string & name, hw_type::iHwType & t);
	// create output signal
	Net & sig_out(const std::string & name, hw_type::iHwType & t);

	/* create constant net in this Netlist
	 * @param v value of the constant net
	 */
	template<typename hw_type_t>
	Net & const_net(hw_type_t & t, typename hw_type_t::value_type::aint_t v) {
		Net & n = sig("const_", t);
		n.val = new typename hw_type_t::value_type(t, v, t.all_mask);
		return n;
	}

	/* create constant net in this Netlist
	 * @param v value of the constant net
	 * @param mask mask for the value
	 */
	template<typename hw_type_t>
	Net & const_net(hw_type_t & t,
			typename hw_type_t::value_type::aint_t v,
			typename hw_type_t::value_type::aint_t mask) {
		Net & n = sig("const_", t);
		n.val = new typename hw_type_t::value_type(t, v, mask);
		return n;
	}
	// create internal signals without specified name
	Net & sig(hw_type::iHwType & t);
	// create internal signal with name specified
	Net & sig(const std::string & name, hw_type::iHwType & t);

	/*
	 * Add node in to nodes on place specified by index and
	 * optionally to nets as well
	 **/
	void register_node(iNode & n);
	void register_node(Net & n);

	/*
	 * Remove node from nodes on place specified by index and
	 * optionally from nets as well
	 *
	 * @note thread safe, but requires compaction to reduce memory footprint
	 **/
	void unregister_node(iNode & n);
	void unregister_node(Net & n);

	void integrty_assert();

	~Netlist();
};

}

