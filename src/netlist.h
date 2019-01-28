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
#include "ordered_set.h"
#include "varId.h"
#include "constants.h"
#include "pointer_container.h"
#include <unordered_map>

namespace netlistDB {

class Netlist;
class Signal;
class Statement;

class Statement {
public:
	Statement * parent;
	Statement() :
			parent(nullptr) {
	}
	virtual ~Statement() {
	}
};

class Assignment: public Statement {
public:
	Signal & dst;
	std::vector<Signal*> dst_index;
	Signal & src;

	Assignment(const Assignment & other) = delete;
	Assignment(Signal & dst, Signal & src);
	Assignment(Signal & dst, std::initializer_list<Signal*> dst_index,
			Signal & src);
};

// definition of function (operator is also function)
class FunctionDef {
public:
	std::string name;
	size_t arg_cnt;

	FunctionDef(const FunctionDef & other) = delete;
	FunctionDef(const std::string & name, size_t arg_cnt);

	Signal & apply(Signal & op0);
	Signal & apply(Signal & op0, Signal & op1);
};

// Container of call of the function (operator is also function)
class FunctionCall {
public:
	// definition of the function
	FunctionDef & fn;
	// arguments specified in call
	std::vector<Signal *> args;
	// result of the function, operator result or return from function call
	Signal & res;

	FunctionCall(const FunctionCall& other) = delete;
	FunctionCall(FunctionDef & fn, Signal & op0, Signal & res);
	FunctionCall(FunctionDef & fn, Signal & op0, Signal & op1, Signal & res);
};

class OperationNode {
public:
	FunctionCall * fnCall;
	Statement * stm;
	OperationNode(const OperationNode& other) = delete;
	OperationNode(FunctionCall * fnCall);

	OperationNode(Statement * stm);

	bool isStm() const;
	bool isFn() const;
	bool operator==(const OperationNode& other) const;
	bool operator!=(const OperationNode& other) const;
	size_t hash() const;
};

/**
 * Hyperedge which connects the
 * */
class Signal {
public:
	// container of name and type
	VarId id;
	// netlist which is the owner of this signal
	Netlist & ctx;

	// direction of the signal if signal is used in IO
	Direction direction;
	// operators/ statements which are driving the value of this signal
	OrderedSet<OperationNode> drivers;
	OrderedSet<OperationNode> endpoints;
	using UsageCacheKey = _UsageCacheKey<FunctionDef, Signal>;
	std::unordered_map<UsageCacheKey, Signal*> usage_cache;
	// index used for last priority ordering
	// represent the sequential number of the signal generated in parent context
	size_t index;

	Signal(const Signal & other) = delete;
	// use methods from Netlist
	Signal(Netlist & ctx, size_t index, const std::string & name,
			Direction direction);

	Signal & operator~();
	Signal & operator|(Signal & other);
	Signal & operator&(Signal & other);
	Signal & operator^(Signal & other);

	Signal & operator<=(Signal & other);
	Signal & operator<(Signal & other);
	Signal & operator>=(Signal & other);
	Signal & operator>(Signal & other);
	Signal & operator==(Signal & other);
	Signal & operator!=(Signal & other);

	Signal & operator-();
	Signal & operator+(Signal & other);
	Signal & operator-(Signal & other);
	Signal & operator*(Signal & other);
	Signal & operator/(Signal & other);

	Signal & concat(Signal & other);
	Signal & rising();
	Signal & falling();

	// as assignment
	Assignment & operator()(Signal & other);
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
 **/
class Netlist {
public:
	// name for debugging purposes
	std::string name;
	std::set<Signal*> signals;
	size_t signal_seq_num;

	Netlist(const Netlist & other) = delete;
	Netlist(const std::string & name);

	// create input signal
	Signal & sig_in(const std::string & name);
	// create output signal
	Signal & sig_out(const std::string & name);

	// create internal signals without specified name
	Signal & sig();
	// create internal signal with name specified
	Signal & sig(const std::string & name);
	~Netlist();
};

}

///////////// hash functions ///////////////////////

namespace std {

template<>
struct hash<netlistDB::OperationNode> {
	typedef netlistDB::OperationNode argument_type;
	typedef size_t result_type;
	result_type operator()(argument_type const& v) const noexcept {
		return v.hash();
	}
};

}
