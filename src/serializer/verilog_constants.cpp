#include <netlistDB/serializer/verilog.h>
#include <netlistDB/operator_defs.h>

using namespace std;

namespace netlistDB {
namespace serializer {

const vector<string> Verilog2001::keywords = { "always"
		"and", "assign", "begin", "buf", "bufif0", "bufif1", "case", "casex",
		"casez", "cmos", "deassign", "default", "defparam", "disable", "edge",
		"else", "end", "endcase", "endfunction", "endmodule", "endprimitive",
		"endspecify", "endtable", "endtask", "event", "for", "force", "forever",
		"fork", "function", "highz0", "highz1", "if", "initial", "inout",
		"input", "integer", "join", "large", "macromodule", "medium", "module",
		"nand", "negedge", "nmos", "nor", "not", "notif0", "notif1", "or",
		"output", "pmos", "posedge", "primitive", "priority", "pull0", "pull1",
		"pulldown", "pullup", "rcmos", "reg", "release", "repeat", "rnmos",
		"rpmos", "rtran", "rtranif0", "rtranif1", "scalared", "small",
		"specify", "specparam", "strong0", "strong1", "supply0", "supply1",
		"table", "task", "time", "tran", "tranif0", "tranif1", "tri", "tri0",
		"tri1", "triand", "trior", "unique", "vectored", "wait", "wand",
		"weak0", "weak1", "while", "wire", "wor", "xnor", "xor", };

const string Verilog2001::DEFAULT_FILE_EXT = ".v";
const std::string Verilog2001::INDENT = "    ";

// http://www.asicguru.com/verilog/tutorial/operators/57/
const map<const FunctionDef*, int> Verilog2001::opPrecedence = {
    //{OpNOT, 3},         //
    {&OpNeg, 5},          //
    {&OpRising, 0},       //
    {&OpDiv, 6},          //
    {&OpAdd, 7},          //
    {&OpSub, 7},          //
    {&OpMul, 3},          //
    {&OpEq, 10},          //
    {&OpNeq, 10},         //
    {&OpAnd, 11},         //
    {&OpXor, 12},         //
    {&OpOr, 13},          //
    //{OpDownto, 2},      //
    {&OpGt, 9},           //
    {&OpLt, 9},           //
    {&OpGE, 9},           //
    {&OpLE, 9},           //
    {&OpConcat, 5},       //
    {&OpSlice, 1},        //
    //{OpTERNARY, 16},    //
    //{OpCALL, 2},        //
};

const map<const FunctionDef*, const string> Verilog2001::_unaryOps = {
    {&OpNeg, "~"},
    //{&OpBitsAsSigned, "$signed(%s)"},
    //{&OpBitsToInt, "%s"},
    //{&OpIntToBits, "%s"},
};

const map<const FunctionDef*, const std::string> Verilog2001::_binOps = {
    {&OpAnd, "&"},
    {&OpOr, "|"},
    {&OpXor, "^"},
    {&OpDiv, "/"},
    //{&OpDownto, ':'},
    //{&OpTo, ':'},
    {&OpEq, "=="},
    {&OpGt, ">"},
    {&OpGE, ">="},
    {&OpLE, "<="},
    {&OpLt, "<"},
    {&OpSub, "-"},
    {&OpMul, "*"},
    {&OpNeq, "!="},
    {&OpAdd, "+"},

    //{&OpPow, '%s ** %s'},
};

const map<const FunctionDef*, function<void(Verilog2001&, const FunctionCall &, ostream & str)>> Verilog2001::_specialOp = {
	{&OpConcat, [](Verilog2001 & s, const FunctionCall & o, ostream & str) {
		str << "{";
		s.serialize_operand(*o.args[0], o, false, false, str);
		str << ", ";
		s.serialize_operand(*o.args[1], o, false, false, str);
		str << "}";
	}},
	{&OpSlice, [](Verilog2001 & s, const FunctionCall & o, ostream & str) {
		s.serialize_operand(*o.args[0], o, true, false, str);
		str << "[" ;
		s.serialize_operand(*o.args[1], o, false, true, str);
		str << "]";
	}},
};


}
}
