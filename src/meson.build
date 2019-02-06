src = [
	'function_call.cpp',
	'function_def.cpp',
	'net.cpp',
	'netlist.cpp',
	'operator_defs.cpp',
	'statement_assignment.cpp',
	'statement_assignment.h',
	'statement_if.cpp',
	'statement_if.h',
	
	'parallel_utils/inclusive_scan.h',
	
	'query/backtrack_context.cpp',
	'query/backtrack_context.h',
	'query/query_traverse.cpp',
	'query/query_traverse.h',
	'query/query_match.cpp',
	'query/query_match.h',
	'query/query_path.cpp',
	'query/query_path.cpp',
	
	'serializer/verilog.cpp',
	'serializer/verilog.h',
	'serializer/namescope.h',
	'serializer/serializer.h',
	
	'transform/remove_useless.cpp',
	'transform/remove_useless.h',
	'transform/transformation.h',
]

EXTRA_CXX_ARGS = [
	'-std=c++17',
 	#'-fopenmp',
]
EXTRA_C_ARGS = []

cc = meson.get_compiler('c')
#libgomp = cc.find_library('gomp')
thread_dep = dependency('threads')
boost_thread = dependency('boost', modules: ['thread', 'system' ])

netlistDB_incldir = include_directories('.')
netlistDB_lib = shared_library('netlistDB',
                        src,
                        include_directories : [netlistDB_incldir, ],
                        dependencies : [boost_thread, thread_dep],
                        install : true,
                        cpp_args: EXTRA_CXX_ARGS,
                        c_args: EXTRA_C_ARGS,
                        #link_args: EXTRA_LINK_ARGS
)
