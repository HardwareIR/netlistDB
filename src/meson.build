src = [
    'debug_utils/timer.cpp',
    'debug_utils/timer.h',

	'hw_type/common.cpp',
	'hw_type/common.h',
	'hw_type/hw_int_value.cpp',
	'hw_type/hw_int_value.h',
	'hw_type/hw_int.cpp',
	'hw_type/hw_int.h',
	'hw_type/ihw_type_value.h',
	'hw_type/ihw_type.cpp',
	'hw_type/ihw_type.h',
	
	'parallel_utils/erase_if.h',
	
	'query/backtrack_context.cpp',
	'query/backtrack_context.h',
	'query/expr_sensitivity_probe.cpp',
	'query/expr_sensitivity_probe.h',
	'query/query_enclosure.cpp',
	'query/query_enclosure.h',
	'query/query_traverse.cpp',
	'query/query_traverse.h',
	'query/query_match.cpp',
	'query/query_match.h',
	'query/query_sensitivity.cpp',
	'query/query_sensitivity.h',
	'query/query_structural_cmp.cpp',
	'query/query_structural_cmp.h',
	'query/query_structural_compatible.cpp',
	'query/query_structural_compatible.h',
	'query/query_path.cpp',
	'query/query_path.cpp',
	
	'serializer/verilog.cpp',
	'serializer/verilog.h',
	'serializer/namescope.h',
	'serializer/serializer.h',
	'transform/indexed_assignment_to_assignment_of_concat.cpp',
	'transform/indexed_assignment_to_assignment_of_concat.h',
	'transform/normalize.cpp',
	'transform/normalize.h',
	'transform/reduce_statement.cpp',
	'transform/reduce_statement.h',
	'transform/remove_by_mask.cpp',
	'transform/remove_by_mask.h',
	'transform/remove_useless.cpp',
	'transform/remove_useless.h',
	'transform/statement_to_hwprocess.cpp',
	'transform/statement_to_hwprocess.h',
	
	'transform/transformation.h',
	
	'function_call.cpp',
	'function_def.cpp',
	'function_def.h',
	'istatement.cpp',
	'net.cpp',
	'netlist.cpp',
	'netlist.h',
	'operator_defs.cpp',
	'statement_assignment.cpp',
	'statement_assignment.h',
	'statement_if.cpp',
	'statement_if.h',
]

# pack all verilog template files in to single cpp with defined strings
ver_dir = 'serializer/verilog/'
templates_to_cpp = find_program('../scripts/templates_to_cpp.py')
templates_as_cpp = custom_target(
	'convert_templates_to_cpp',
    input: [ver_dir + 'component_instance.v', ver_dir + 'if.v',
            ver_dir + 'module_body.v', ver_dir + 'module_head.v',
            ver_dir + 'process.v', ver_dir + 'switch.v'],
    output: ['verilog_templates.cpp'],
    command: [templates_to_cpp, '@INPUT@', '@OUTPUT@'])
src += templates_as_cpp

EXTRA_CXX_ARGS = [
	'-std=c++17',
 	#'-fopenmp',
]
EXTRA_C_ARGS = []

#cc = meson.get_compiler('c')
#libgomp = cc.find_library('gomp')
#boost_thread = dependency('boost', modules: ['thread', 'system'])
thread_dep = dependency('threads')
tbb_dep = dependency('tbb')
netlistDB_incldir = include_directories('.')
netlistDB_lib = shared_library(
	'netlistDB',
    src,
    include_directories : [netlistDB_incldir, ],
    dependencies : [thread_dep, tbb_dep],
    install : true,
    cpp_args: EXTRA_CXX_ARGS,
    c_args: EXTRA_C_ARGS,
    #link_args: EXTRA_LINK_ARGS
)
