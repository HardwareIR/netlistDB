src = [
	'assignment.cpp',
	'function_call.cpp',
	'function_def.cpp',
	'net.cpp',
	'netlist.cpp',
	'operator_defs.cpp',
	'operation_node.cpp',
	'statement_if.cpp',
	
	'query/backtrack_context.cpp',
	'query/backtrack_context.h',
	'query/query_flooding.cpp',
	'query/query_flooding.h',
	'query/query_path.cpp',
	'query/query_path.cpp',
	'query/query_match.cpp',
	'query/query_match.h',
	
	'serializer/verilog.cpp',
	'serializer/verilog.h',
	'serializer/namescope.h',
	'serializer/serializer.h',
]

EXTRA_CXX_ARGS = [
	'-std=c++17',
 	'-fopenmp',
]
EXTRA_C_ARGS = []

cc = meson.get_compiler('c')
libgomp = cc.find_library('gomp')
boost_thread = dependency('boost', modules: ['thread', 'system' ])

netlistDB_incldir = include_directories('.')
netlistDB_lib = shared_library('netlistDB',
                        src,
                        include_directories : [netlistDB_incldir, ],
                        dependencies : [boost_thread],
                        install : true,
                        cpp_args: EXTRA_CXX_ARGS,
                        c_args: EXTRA_C_ARGS,
                        #link_args: EXTRA_LINK_ARGS
)
