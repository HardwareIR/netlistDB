#include "operator_defs.h"

namespace netlistDB {

FunctionDef OpNot("not", 1);
FunctionDef OpAnd("and", 2);
FunctionDef OpOr("or", 2);
FunctionDef OpXor("xor", 2);
// nand, reverse

FunctionDef OpLE("<=", 2);
FunctionDef OpGE(">=", 2);
FunctionDef OpGt(">", 2);
FunctionDef OpLt("<", 2);
FunctionDef OpEq("==", 2);
FunctionDef OpNeq("!=", 2);

// arithmetic

FunctionDef OpUnMinus("-", 1);
FunctionDef OpNeg("~", 2);
FunctionDef OpAdd("+", 2);
FunctionDef OpSub("-", 2);
FunctionDef OpDiv("/", 2);
FunctionDef OpMul("*", 2);
// modulo, rem?

FunctionDef OpConcat("concat", 2);

FunctionDef OpRising("rising", 2);
FunctionDef OpFalling("falling", 2);

}
