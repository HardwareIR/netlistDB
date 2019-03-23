src = [
	'hw_type/aint.cpp',
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
	'serializer/serializer_io.cpp',
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
    'utils/timer.cpp',

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

EXTRA_CXX_ARGS = []
EXTRA_C_ARGS = []

if meson.get_compiler('cpp').get_id() == 'msvc'
	if get_option('default_library') == 'static'
		EXTRA = ['/DBOOST_ALL_NO_LIB',
        	     #'/DBOOST_ALL_DYN_LINK',
        	     ]
	else
		EXTRA = []
	endif
    EXTRA += [#'/wd4251', # "needs to have dll-interface to be used by clients of class"
             #'/DNETLISTDB_EXPORTS', '/DNETLISTDB_DLL',
             '/D_WIN32_WINNT=0x0501' # windows XP+
             ]
	EXTRA_CXX_ARGS += EXTRA
	EXTRA_C_ARGS += EXTRA
endif

_boost_dep = dependency('boost', modules: ['system', 'thread', 'filesystem'])
_netlistDB_inc = include_directories('../include/')
_netlistDB_lib = library(
	'netlistDB',
    src,
    include_directories : [_netlistDB_inc, ],
    dependencies : [_boost_dep],
    install : true,
    cpp_args: EXTRA_CXX_ARGS,
    c_args: EXTRA_C_ARGS,
    #link_args: EXTRA_LINK_ARGS
)
install_subdir('../include/netlistDB', install_dir : 'include')
netlistDB_dep = declare_dependency(
  dependencies: [_boost_dep],
  link_with : _netlistDB_lib,
  include_directories : _netlistDB_inc
)

