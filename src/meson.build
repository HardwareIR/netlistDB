src = [
    'debug_utils/timer.cpp',

	'hw_type/common.cpp',
	'hw_type/hw_int_value.cpp',
	'hw_type/hw_int.cpp',
	'hw_type/ihw_type.cpp',
	
	'query/backtrack_context.cpp',
	'query/expr_sensitivity_probe.cpp',
	'query/query_enclosure.cpp',
	'query/query_traverse.cpp',
	'query/query_match.cpp',
	'query/query_public_net.cpp',
	'query/query_sensitivity.cpp',
	'query/query_structural_cmp.cpp',
	'query/query_structural_compatible.cpp',
	'query/query_path.cpp',
	'query/statement_visitor.cpp',
	
	'serializer/namescope.cpp',
	'serializer/serializer.cpp',
	'serializer/verilog_constants.cpp',
	'serializer/verilog_ops.cpp',
	'serializer/verilog_statements.cpp',
	'serializer/verilog_structural.cpp',
	'serializer/verilog_tmp_var_extractor.cpp',
	'serializer/verilog.cpp',
	
	'transform/indexed_assignment_to_assignment_of_concat.cpp',
	'transform/mark_inter_process_nets.cpp',
	'transform/normalize.cpp',
	'transform/reduce_statement.cpp',
	'transform/remove_by_mask.cpp',
	'transform/remove_useless.cpp',
	'transform/statement_to_hwprocess.cpp',
	'transform/to_hdl_friendly.cpp',
	
	'utils/thread_pool.cpp',

	'bit_utils.cpp',
	'component_map.cpp',
	'function_call.cpp',
	'function_def.cpp',
	'istatement.cpp',
	'net.cpp',
	'netlist.cpp',
	'operator_defs.cpp',
	'statement_assignment.cpp',
	'statement_hwprocess.cpp',
	'statement_if.cpp',
]

# pack all verilog template files in to single cpp with defined strings
ver_dir = 'serializer/verilog/'

EXTRA_CXX_ARGS = []
EXTRA_C_ARGS = []

#thread_dep = dependency('threads')
boost_deps = dependency('boost', modules: ['system', 'thread'])

netlistDB_incldir = include_directories('../include/')
netlistDB_lib = shared_library(
	'netlistDB',
    src,
    include_directories : [netlistDB_incldir, ],
    dependencies : [boost_deps],
    install : true,
    cpp_args: EXTRA_CXX_ARGS,
    c_args: EXTRA_C_ARGS,
    #link_args: EXTRA_LINK_ARGS
)
install_subdir('../include/netlistDB', install_dir : 'include')
