#include "netlist.h"

using namespace netlistDB;

Assignment::Assignment(Net & dst, Net & src) :
		Statement(), dst(dst), src(src) {

}

Assignment::Assignment(Net & dst, std::initializer_list<Net*> dst_index,
		Net & src) :
		dst(dst), dst_index(dst_index), src(src) {
}
