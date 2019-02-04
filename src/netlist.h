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

#include "ordered_set.h"
#include "varId.h"
#include "constants.h"
#include "pointer_container.h"
#include "function_def.h"

namespace netlistDB {

class Net;

/**
 * Inteface for nodes in database
 *
 **/
class iNode {
public:
	// sequential number used as a id during serialization
	size_t index;

	iNode(size_t index) :
			index(index) {
	}

	using iterator = std::vector<iNode*>;
	// iterate endpoints for Net or outputs for statement or result for expression
	virtual iterator forward() = 0;
	// iterate drivers for net or inputs for statement or args for expression
	virtual iterator backward() = 0;
	virtual ~iNode() {
	}
};

class Netlist;
class Statement;

class OperationNode: public iNode {
public:
	OperationNode(size_t index) :
			iNode(index) {
	}
};

class Statement: public OperationNode {
public:
	Statement * parent;

	Statement(size_t index) :
			OperationNode(index), parent(nullptr) {
	}
};

class Assignment: public Statement {
public:
	Net & dst;
	std::vector<Net*> dst_index;
	Net & src;

	Assignment(const Assignment & other) = delete;
	Assignment(Net & dst, Net & src);
	Assignment(Net & dst, std::initializer_list<Net*> dst_index, Net & src);
	virtual iNode::iterator forward() override;
	virtual iNode::iterator backward() override;
};

// Container of call of the function (operator is also function)
class FunctionCall: public OperationNode {
public:
	// definition of the function
	FunctionDef & fn;
	// arguments specified in call
	std::vector<Net *> args;
	// result of the function, operator result or return from function call
	Net & res;

	FunctionCall(const FunctionCall& other) = delete;
	FunctionCall(size_t index, FunctionDef & fn, Net & op0, Net & res);
	FunctionCall(size_t index, FunctionDef & fn, Net & op0, Net & op1,
			Net & res);

	virtual iNode::iterator forward() override;
	virtual iNode::iterator backward() override;
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

	// direction of the signal if signal is used in IO
	Direction direction;
	// operators/ statements which are driving the value of this signal
	OrderedSet<OperationNode*> drivers;
	OrderedSet<OperationNode*> endpoints;
	using UsageCacheKey = _UsageCacheKey<FunctionDef, Net>;
	std::unordered_map<UsageCacheKey, Net*> usage_cache;
	// index used for last priority ordering
	// represent the sequential number of the signal generated in parent context
	size_t index;

	Net(const Net & other) = delete;
	// use methods from Netlist
	Net(Netlist & ctx, size_t index, const std::string & name,
			Direction direction);

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

	Net & concat(Net & other);
	Net & rising();
	Net & falling();

	// as assignment
	Assignment & operator()(Net & other);

	virtual iNode::iterator forward() override;
	virtual iNode::iterator backward() override;
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
	std::set<Net*> nets;
	size_t obj_seq_num;

	Netlist(const Netlist & other) = delete;
	Netlist(const std::string & name);

	// create input signal
	Net & sig_in(const std::string & name);
	// create output signal
	Net & sig_out(const std::string & name);

	// create internal signals without specified name
	Net & sig();
	// create internal signal with name specified
	Net & sig(const std::string & name);
	~Netlist();
};

}

