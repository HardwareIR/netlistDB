src = [
	'assignment.cpp',
	'function_call.cpp',
	'function_def.cpp',
	'netlist.cpp',
	'operator_defs.cpp',
	'operation_node.cpp',
	'signal.cpp',
	'statement_if.cpp',
]

EXTRA_CXX_ARGS = []
EXTRA_C_ARGS = []

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
