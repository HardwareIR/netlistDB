project('netlistDB', ['c', 'cpp'], default_options : ['cpp_std=c++14', ])

subdir('src')
subdir('tests')

pkg_mod = import('pkgconfig')
pkg_mod.generate(libraries : _netlistDB_lib,
                 version : '0.1',
                 name : 'netlistDB',
                 description : 'A Library for the representation and manipulation with netlist.')