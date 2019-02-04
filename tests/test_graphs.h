#pragma once

#include <vector>
#include <functional>
#include <random>
#include <map>

#include "../src/netlist.h"
#include "../src/function_def.h"
#include "../src/operator_defs.h"

void build_random_circuit(netlistDB::Netlist & ctx, size_t input_cnt,
		size_t output_cnt, size_t height, size_t connection_per_level,
		std::mt19937 & rand);
