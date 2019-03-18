#include <netlistDB/operator_defs.h>

namespace netlistDB {

//FunctionDef OpNot("not", 1);
NETLISTDB_PUBLIC FunctionDef OpNeg("~", 1);
NETLISTDB_PUBLIC FunctionDef OpAnd("and", 2);
NETLISTDB_PUBLIC FunctionDef OpOr("or", 2);
NETLISTDB_PUBLIC FunctionDef OpXor("xor", 2);

NETLISTDB_PUBLIC FunctionDef OpLE("<=", 2);
NETLISTDB_PUBLIC FunctionDef OpGE(">=", 2);
NETLISTDB_PUBLIC FunctionDef OpGt(">", 2);
NETLISTDB_PUBLIC FunctionDef OpLt("<", 2);
NETLISTDB_PUBLIC FunctionDef OpEq("==", 2);
NETLISTDB_PUBLIC FunctionDef OpNeq("!=", 2);

NETLISTDB_PUBLIC FunctionDef OpUnMinus("-", 1);
NETLISTDB_PUBLIC FunctionDef OpAdd("+", 2);
NETLISTDB_PUBLIC FunctionDef OpSub("-", 2);
NETLISTDB_PUBLIC FunctionDef OpDiv("/", 2);
NETLISTDB_PUBLIC FunctionDef OpMul("*", 2);

NETLISTDB_PUBLIC FunctionDef OpConcat("concat", 2);
NETLISTDB_PUBLIC FunctionDef OpSlice("slice", 2);
NETLISTDB_PUBLIC FunctionDef OpDownto("downto", 2);

NETLISTDB_PUBLIC FunctionDef OpRising("rising", 1);
NETLISTDB_PUBLIC FunctionDef OpFalling("falling", 1);

}
