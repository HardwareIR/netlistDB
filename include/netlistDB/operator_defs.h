#pragma once

#include <netlistDB/netlist.h>

namespace netlistDB {

// bitwise
//extern FunctionDef OpNot;
NETLISTDB_PUBLIC extern FunctionDef OpNeg;
NETLISTDB_PUBLIC extern FunctionDef OpAnd;
NETLISTDB_PUBLIC extern FunctionDef OpOr;
NETLISTDB_PUBLIC extern FunctionDef OpXor;
// nand, reverse

// cmp
NETLISTDB_PUBLIC extern FunctionDef OpLE;
NETLISTDB_PUBLIC extern FunctionDef OpGE;
NETLISTDB_PUBLIC extern FunctionDef OpGt;
NETLISTDB_PUBLIC extern FunctionDef OpLt;
NETLISTDB_PUBLIC extern FunctionDef OpEq;
NETLISTDB_PUBLIC extern FunctionDef OpNeq;


// arithmetic
NETLISTDB_PUBLIC extern FunctionDef OpUnMinus;
NETLISTDB_PUBLIC extern FunctionDef OpAdd;
NETLISTDB_PUBLIC extern FunctionDef OpSub;
NETLISTDB_PUBLIC extern FunctionDef OpDiv;
NETLISTDB_PUBLIC extern FunctionDef OpMul;
// modulo, rem?

NETLISTDB_PUBLIC extern FunctionDef OpConcat;
NETLISTDB_PUBLIC extern FunctionDef OpSlice; // index, ranged index
NETLISTDB_PUBLIC extern FunctionDef OpDownto;

NETLISTDB_PUBLIC extern FunctionDef OpRising;
NETLISTDB_PUBLIC extern FunctionDef OpFalling;
// [todo] rising and falling ?= event

constexpr bool is_event_op(const FunctionDef & op) {
	return &op == &OpRising or &op == &OpFalling;
}

}
