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
#include <map>

#include <netlistDB/inode.h>
#include <netlistDB/utils/ordered_set.h>
#include <netlistDB/varId.h>
#include <netlistDB/constants.h>
#include <netlistDB/function_def.h>
#include <netlistDB/hw_type/ihw_type.h>
#include <netlistDB/hw_type/ihw_type_value.h>
#include <netlistDB/utils/sensitivity_ctx.h>
#include <netlistDB/usage_cache_key.h>

namespace netlistDB {

class NETLISTDB_PUBLIC Net;
class NETLISTDB_PUBLIC Netlist;
class NETLISTDB_PUBLIC Statement;

class NETLISTDB_PUBLIC OperationNode: public iNode {
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
class NETLISTDB_PUBLIC SensitivityInfo {
public:
	bool is_completly_event_dependent;
	bool now_is_event_dependent;
	utils::OrderedSet<Net*> enclosed_for;
	utils::SensitivityCtx sensitivity;
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
class NETLISTDB_PUBLIC Statement: public OperationNode {
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
	 * walk all direct child statements with specified function fn
	 *
	 * @param fn callback which is called on each node, if the function returns true the iteration is stoped
	 * 		and the visit_child_stm returns
	 * */
	virtual void visit_child_stm(const std::function<bool(Statement &)> & fn) = 0;

	/*
	 * @return the Netlist where signals for connected to this statement are created
	 * */
	Netlist & _get_context();

	/*
	 * call _set_parent_stm for the collection and copy the vector to target
	 * */
	void _register_stements(const std::vector<Statement*> & statements,
			std::vector<Statement*> & target);

	virtual void forward(const predicate_t & fn) override;
	virtual void backward(const predicate_t & fn) override;
};

/**
 * Container of call of the function (operator is also function)
 */
class NETLISTDB_PUBLIC FunctionCall: public OperationNode {
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

	virtual void forward(const predicate_t & fn) override;
	virtual void backward(const predicate_t & fn) override;
};

/*
 * Code construct which connects the nets to the ports of the child component
 * */
class NETLISTDB_PUBLIC ComponentMap: public OperationNode {
public:
	// backward reference to the Netlist instance
	// where this component is instantiated
	Netlist & parent;
	// ptr on Netlist which is instantiated
	const std::shared_ptr<Netlist> component;
	// the direction of the connection is resolved by direction of the net (port) in child component
	std::map<Net*, Net*> parent_to_child;
	std::map<Net*, Net*> child_to_parent;

	// @note you can create shared_ptr from local variable using null_deleter
	//        std::shared_ptr<Netlist>(&m, [](Netlist*){})
	ComponentMap(Netlist & parent, std::shared_ptr<Netlist> component);

	void connect(Net & parent_net, Net & component_port);

	virtual void forward(const predicate_t & fn) override;
	virtual void backward(const predicate_t & fn) override;
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
class NETLISTDB_PUBLIC Netlist {
public:
	// name for serialization and debugging
	std::string name;
	// nets (and ports) in this netlist (children netlists not included)
	std::vector<Net*> nets;
	// all nets, statements, operators, etc.
	std::vector<iNode*> nodes;

	Netlist(const Netlist & other) = delete;
	Netlist(const std::string & name);

	// create input signal
	Net & sig_in(const std::string & name, hw_type::iHwType & t);
	// create output signal
	Net & sig_out(const std::string & name, hw_type::iHwType & t);

	// create internal signals without specified name
	Net & sig(hw_type::iHwType & t);
	// create internal signal with name specified
	Net & sig(const std::string & name, hw_type::iHwType & t);

	// add sub component
	ComponentMap & add_component(std::shared_ptr<Netlist> component);
	ComponentMap & add_component(Netlist & component);

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

/** The net in Netlist instance also the hyperedge which connects
 *  the statements, expressions, etc.
 *
 *  @note The overloaded operators are building the expression in the netlist
 * */
class NETLISTDB_PUBLIC Net: public iNode {
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
	// bitwise operators
	Net & operator~();
	Net & operator|(Net & other);
	template<typename T>
	Net & operator|(T val) {
		return (*this) | wrap_val_to_const_net(val);
	}
	Net & operator&(Net & other);
	template<typename T>
	Net & operator&(T val) {
		return (*this) & wrap_val_to_const_net(val);
	}
	Net & operator^(Net & other);
	template<typename T>
	Net & operator^(T val) {
		return (*this) ^ wrap_val_to_const_net(val);
	}
	// cmp operators
	Net & operator<=(Net & other);
	template<typename T>
	Net & operator<=(T val) {
		return (*this) <= wrap_val_to_const_net(val);
	}
	Net & operator<(Net & other);
	template<typename T>
	Net & operator<(T val) {
		return (*this) < wrap_val_to_const_net(val);
	}
	Net & operator>=(Net & other);
	template<typename T>
	Net & operator>=(T val) {
		return (*this) >= wrap_val_to_const_net(val);
	}
	Net & operator>(Net & other);
	template<typename T>
	Net & operator>(T val) {
		return (*this) > wrap_val_to_const_net(val);
	}
	Net & operator==(Net & other);
	template<typename T>
	Net & operator==(T val) {
		return (*this) == wrap_val_to_const_net(val);
	}
	Net & operator!=(Net & other);
	template<typename T>
	Net & operator!=(T val) {
		return (*this) != wrap_val_to_const_net(val);
	}

	// arithmetic operators
	Net & operator-();
	Net & operator+(Net & other);
	template<typename T>
	Net & operator+(T val) {
		return (*this) + wrap_val_to_const_net(val);
	}
	Net & operator-(Net & other);
	template<typename T>
	Net & operator-(T val) {
		return (*this) - wrap_val_to_const_net(val);
	}
	Net & operator*(Net & other);
	template<typename T>
	Net & operator*(T val) {
		return (*this) * wrap_val_to_const_net(val);
	}
	Net & operator/(Net & other);
	template<typename T>
	Net & operator/(T val) {
		return (*this) / wrap_val_to_const_net(val);
	}

	Net & operator[](Net & index);
	template<typename T>
	Net & operator[](T val) {
		return (*this)[wrap_val_to_const_net(val)];
	}

	Net & concat(Net & other);
	Net & rising();
	Net & falling();

	// as assignment
	Statement & operator()(Net & other);
	template<typename T>
	Statement & operator()(T val) {
		return (*this)(wrap_val_to_const_net(val));
	}

	virtual void forward(const predicate_t & fn) override;
	virtual void backward(const predicate_t & fn) override;
	// disconnect the selected endpoints
	void forward_disconnect(std::function<bool(iNode*)> pred);

	virtual ~Net() override;
private:
	/*
	 * Wrap c-constant to constant net in parent netlist
	 * */
	Net & wrap_val_to_const_net(uint64_t val);
	Net & wrap_val_to_const_net(int64_t val);
	Net & wrap_val_to_const_net(int val);
	Net & wrap_val_to_const_net(unsigned val);
};





}

