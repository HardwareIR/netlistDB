
boost_deps = dependency('boost', modules: ['unit_test_framework', ])

tests = [
	  ['basic', 'test of the core'],
	  ['basic_query_match', 'basic test for query engine'],
	  ['large_query_match', 'benchmarks for query engine']
]

foreach t : tests
	test_exe = executable(t[0]  + '_test',
		[t[0] + '_test.cpp'],
		dependencies: [ boost_deps],
		link_with: [netlistDB_lib, ],
		cpp_args: EXTRA_CXX_ARGS,
        c_args: EXTRA_C_ARGS)
	test(t[1] + '_test', test_exe, workdir : meson.source_root())
endforeach