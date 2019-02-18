#pragma once

#include <vector>
#include <functional>
#include <random>
#include <map>

#include <netlistDB/netlist.h>
#include <netlistDB/function_def.h>
#include <netlistDB/operator_defs.h>

void build_random_circuit(netlistDB::Netlist & ctx, size_t input_cnt,
		size_t output_cnt, size_t height, size_t connection_per_level,
		std::mt19937 & rand);
