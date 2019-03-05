
test_boost_deps = dependency('boost', modules: ['unit_test_framework', 'thread', 'system' ])

tests = [
	  ['basic', 'test of the core'],
	  ['basic_query_match', 'basic test for query engine'],
	  ['large_query_match', 'benchmarks for query engine'],
	  ['chained_iter', 'tests for chained iterator'],
	  ['ordered_set', 'tests for ordered set collection'],
	  ['query_enclosure', 'test for the QueryEnclosure'],
	  ['query_flooding', 'tests for flooding query engine'],
	  ['query_sensitivity', 'tests for QuerySensitivity'],
	  ['query_structural_cmp', 'tests for QueryStructuralCmp'],
  	  ['query_traverse', 'tests for QueryTraverse'],
  	  ['erase_if', 'tests for erase if'],
	  ['reduce_statement', 'tests for TransformReduceStatement'],
	  ['statement_assignment', 'test for statment Assignment object'],
	  ['statement_if', 'test for IfStatemet object'],
	  ['expr_sensitivity_probe', 'test for the epxression sensitivity probe'],
	  ['statement_to_hwprocess', 'test for TransformStatementToHwProcess'],
      ['thread_pool', 'tests for the utils::ThreadPool'],
	  ['transform_remove_useless', 'tests for TransformRemoveUseless'],
	  ['unsigned_val', 'tests for unsigned arithmetic'],
	  ['verilog_basic', 'basic tests for verilog serializer'],
	  ['verilog_simple_modules', 'verilog serializer tests with simple modules'],
]

test_common_src = [
	'test_graphs.cpp',
	'test_graphs.h',
]
foreach t : tests
	test_exe = executable(t[0]  + '_test',
		[t[0] + '_test.cpp'] + test_common_src,
		dependencies: [test_boost_deps],
		link_with: [netlistDB_lib, ],
		cpp_args: EXTRA_CXX_ARGS,
        c_args: EXTRA_C_ARGS,
        include_directories: netlistDB_incldir)
	test(t[1], test_exe, workdir : meson.source_root())
endforeach

#igraph_dep = dependency('igraph')
#igraph_benchmark_exe = executable('igraph_benchmark',
#		['igraph_benchmark.cpp', 'netlist_to_igraph.cpp',
#		 'netlist_to_igraph.h'] + test_common_src,
#		dependencies: [boost_deps, thread_dep, igraph_dep],
#		link_with: [netlistDB_lib, ],
#		cpp_args: EXTRA_CXX_ARGS,
#        c_args: EXTRA_C_ARGS)

