#include <netlistDB/transform/indexed_assignment_to_assignment_of_concat.h>

#include <exception>
#include <thread>
#include <netlistDB/statement_assignment.h>

using namespace std;

namespace netlistDB {
namespace transform {

pair<size_t, size_t> net_val_as_range(Net & n) {
	throw runtime_error("not implemented");
}

bool TransformIndexedAssignmentsToAssignmentOfConcat::is_result_of_indexed_assignment(
		Net & sig, vector<Net*> & inputs) {
	inputs.clear();
	if (sig.drivers.size() == 0)
		return false;

	vector<range_and_src_t> match;
	// note: a[0] = b is also replaced case
	for (auto d : sig.drivers) {
		auto assig = dynamic_cast<Assignment*>(d);
		if (assig != nullptr and assig->dst_index.size() > 0) {
			bool all_indx_const = true;
			for (auto i : assig->dst_index) {
				if (not i->is_const()) {
					all_indx_const = false;
					break;
				}
			}
			if (all_indx_const) {
				if (assig->dst_index.size() > 1)
					throw std::runtime_error("not implemented");
				auto range = net_val_as_range(*assig->dst_index[0]);
				match.push_back( { range, &assig->dst });
			}
		} else {
			break;
		}
	}

	if (sig.drivers.size() != match.size()) {
		match.clear();
		return false;
	} else {
		// Sort items to correct order
		sort(match.begin(), match.end(),
				[](const range_and_src_t & a, const range_and_src_t & b) -> bool {
					return a.first.first > b.first.first;
				});

		// Assert the parts are not overlapping
		// and whole signal is driven
		size_t expected_next_start = 0;
		for (size_t i = 0; i < match.size(); i++) {
			auto r = match[i].first;
			assert(r.first == expected_next_start);
			assert(r.first <= r.second);
			expected_next_start = r.second + 1;
		}
		assert(
				sig.t.bit_length() == expected_next_start
						&& "All parts of the signal are driven");

		for (auto m : match) {
			inputs.push_back(m.second);
		}

		return true;
	}
}

bool TransformIndexedAssignmentsToAssignmentOfConcat::apply(Netlist & ctx) {
	throw std::runtime_error("not implemented");
	// walk all signals

	size_t thread_cnt = thread::hardware_concurrency();
	auto to_update = make_unique<vector<pair<Net*, vector<Net*> > > []>(
			thread_cnt);
	// If the signal is driven by multiple combinational assignments to parts of this signal
	// Assert all parts of the signals are driven

	// Collect all the input signals for specified target signal
	thread_pool.task_group().parallel_for(size_t(0), size_t(thread_cnt),
			[&] (const size_t thread_i) {
				size_t step = thread_cnt;
				auto & to_upd = to_update[thread_i];

				for(size_t i = thread_i; i < ctx.nets.size(); i+= step) {
					Net * n = ctx.nets[i];
					vector<Net*> inputs;
					if (is_result_of_indexed_assignment(*n, inputs)) {
						to_upd.push_back( {n, inputs});
					}
				}

			});

	bool updated = false;
	// sequentially replace the indexed assignments
	for (size_t t = 0; t < thread_cnt; t++) {
		for (auto rec : to_update[t]) {
			// delete all drivers as they are being replaced
			// create concatenation of all inputs
			// connect concatenated inputs to output
			throw std::runtime_error("not implemented");
			updated = true;
		}
	}

	return updated;
}

}
}
