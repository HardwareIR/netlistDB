#include "netlist.h"

using namespace netlistDB;

FunctionCall::FunctionCall(FunctionDef & fn, Signal & arg0, Signal & res) :
		fn(fn), args( { &arg0 }), res(res) {

}

FunctionCall::FunctionCall(FunctionDef & fn, Signal & arg0, Signal & arg1,
		Signal & res) :
		fn(fn), args( { &arg0, &arg1 }), res(res) {
}
