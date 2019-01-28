#include "netlist.h"

using namespace netlistDB;

Assignment::Assignment(Signal & dst, Signal & src) :
		Statement(), dst(dst), src(src) {

}

Assignment::Assignment(Signal & dst, std::initializer_list<Signal*> dst_index,
		Signal & src) :
		dst(dst), dst_index(dst_index), src(src) {
}
