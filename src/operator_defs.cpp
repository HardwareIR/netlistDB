#include <netlistDB/operator_defs.h>

namespace netlistDB {

//FunctionDef OpNot("not", 1);
FunctionDef OpNeg("~", 1);
FunctionDef OpAnd("and", 2);
FunctionDef OpOr("or", 2);
FunctionDef OpXor("xor", 2);

FunctionDef OpLE("<=", 2);
FunctionDef OpGE(">=", 2);
FunctionDef OpGt(">", 2);
FunctionDef OpLt("<", 2);
FunctionDef OpEq("==", 2);
FunctionDef OpNeq("!=", 2);

FunctionDef OpUnMinus("-", 1);
FunctionDef OpAdd("+", 2);
FunctionDef OpSub("-", 2);
FunctionDef OpDiv("/", 2);
FunctionDef OpMul("*", 2);

FunctionDef OpConcat("concat", 2);
FunctionDef OpSlice("slice", 2);

FunctionDef OpRising("rising", 1);
FunctionDef OpFalling("falling", 1);

}
