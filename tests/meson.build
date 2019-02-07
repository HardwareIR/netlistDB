
boost_deps = dependency('boost', modules: ['unit_test_framework', ])

tests = [
	  ['basic', 'test of the core'],
	  ['basic_query_match', 'basic test for query engine'],
	  ['large_query_match', 'benchmarks for query engine'],
	  ['query_flooding', 'tests for flooding query engine'],
	  ['taskflow', 'tests for taskflow functionality'],
	  ['transform_remove_useless', 'tests for TransformRemoveUseless functionality'],
	  ['chained_iter', 'tests for chained iterator'],
]

test_common_src = [
	'test_graphs.cpp',
	'test_graphs.h',
]

foreach t : tests
	test_exe = executable(t[0]  + '_test',
		[t[0] + '_test.cpp'] + test_common_src,
		dependencies: [boost_deps, thread_dep],
		link_with: [netlistDB_lib, ],
		cpp_args: EXTRA_CXX_ARGS,
        c_args: EXTRA_C_ARGS)
	test(t[1] + '_test', test_exe, workdir : meson.source_root())
endforeach

#tbb_dep = dependency('tbb')
#tbb_taskflow_benchmark_exe = executable('tbb_taskflow_benchmark',
#		['tbb_taskflow_benchmark.cpp'] + test_common_src + ['timer.cpp'],
#		dependencies: [boost_deps, thread_dep, tbb_dep],
#		link_with: [netlistDB_lib, ],
#		cpp_args: EXTRA_CXX_ARGS,
#        c_args: EXTRA_C_ARGS)
