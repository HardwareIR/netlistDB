#pragma once

namespace netlistDB {
namespace serializer {

class Serializer {
public:
	virtual void serialize(Netlist & netlist, std::ostream & str) = 0;
	virtual ~Serializer() {
	}
};

}

}
