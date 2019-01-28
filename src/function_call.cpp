#include "netlist.h"

using namespace netlistDB;

FunctionCall::FunctionCall(FunctionDef & fn, Net & arg0, Net & res) :
		fn(fn), args( { &arg0 }), res(res) {

}

FunctionCall::FunctionCall(FunctionDef & fn, Net & arg0, Net & arg1,
		Net & res) :
		fn(fn), args( { &arg0, &arg1 }), res(res) {
}
