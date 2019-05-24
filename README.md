# NetlistDB 
[![Build Status](https://travis-ci.org/HardwareIR/netlistDB.svg?branch=master)](https://travis-ci.org/HardwareIR/netlistDB)
[![Windows build](https://ci.appveyor.com/api/projects/status/9a9ckao995bcetbk/branch/master?svg=true)](https://ci.appveyor.com/project/Nic3084362/netlistdb/branch/master)
[![Coverage Status](https://coveralls.io/repos/github/HardwareIR/hardwareIr/badge.svg?branch=master)](https://coveralls.io/github/HardwareIR/hardwareIr?branch=master)
[![Gitter Chat](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/netlistDB/community)
[![Doc](https://readthedocs.org/projects/netlistdb/badge/?version=latest)](http://netlistdb.readthedocs.io/en/latest/?badge=latest) 
[ROADMAP](https://drive.google.com/file/d/1zyegLIf7VaBRyb-ED5vgOMmHzW4SRZLp/view?usp=sharing)

(State: alfa)

NetlistDB is an experimental netlist database for hardware developement tools (hardware synthesis tools, simulators, code generators, ...).
It is designed to allow efficient parallel processing of complex circut transformation tasks while not sacrificing single thread performance.

## The purpose of NetlistDB

This library is a backend and intermediate format for representation of digital circuit. The direct use may result in more ugly code than verilog itself. Think of this as blazing fast code generator and the backend for the circuit optimalizers and generators. And also as a glue which connects various libraries related to HW development.

# Circuit representation in NetlistDB


|                         | Verilog   | VHDL                  |  NetlistDB                                    |
|:------------------------|-----------|-----------------------|-----------------------------------------------|
| net(Signal)             | wire      | signal                |  Net object - construt by Netlist.sig()       |
| component               | module    | entity + architecture |  Netlist object                               |
| operators               | +/-, ...  | +/-, ...              |  FunctionCall object - use overrided Net ops. |
| function calls          | fn()      | fn()                  |  FunctionCall object                          |
| if statement            | if        | if                    |  IfStatement object - use If function         |
| blocking assignment     | =         | :=                    |                                               |
| non-blocking assignment | <=        | <=                    |  Assignment object - use Net::operator() [1]  |
| preprocessor            | macros    | generate              |  c++ code - no restrictions                   |
| process, sensitivity    | always,.. | process               |  automatically resolved [2]                   |
| bits type               | [0:15]    | std_logic_vector      | HwInt object                                  |

[1] The Assignment object constructed by call operator on Net is non blocking as blocking assingment can be used the c++ assignment.

[2] Each statement is self sufficient process with automatically managed sensitivity. (There is transformation class which transforms the statements to readable format so the serialized output is minimalized and readable.)
    

# Installation

Install dependencies
```
sudo apt-get install build-essentials meson ninja-build libboost-all-dev git
# [NOTE] if you are using old system like like ubuntu 14 on travis-ci install fresh meson by
#        pip3 install meson instead to avoid complications with libboost discovery
```

build this library
```
git clone https://github.com/HardwareIR/netlistDB
cd netlistDB
meson build # meson is tool similar to cmake or autotools
cd build
ninja # ninja is tool similar to make
```

run tests
```
ninja test
```

install netlistDB
```
sudo ninja install # ninja uninstall is also available and DESTDIR=/staging ninja install also works
```



# Structure of NetlistDB

The main object in NetlistDB is netlistDB::Netlist this object is a container of Net objects.
This object can be used to build the circuits as it is shown in example below.

```cpp
Netlist ctx("adder");
Net &a = ctx.sig_in("a", hw_int32);
Net &b = ctx.sig_in("b", hw_int32);
Net &res_tmp = a + b;
Net &res = ctx.sig_out("out", hw_int32);
res(res_tmp); // equivalent of res <= res_tmp in verilog/vhdl

// convert the ctx to verilog string
TransformToHdlFriendly t;
t.apply(ctx);
Verilog2001 ser(std::cout);
ser.serialize(ctx);
```

To perform analysis of the Netlist there is netlistDB::query namespace.
The netlistDB::query::QueryMatch can be used to perform graph pattern match (isomorphic graph search).
The QueryMatch is also Netlist. The query is specified by the other circuit as shown in example below.

```cpp
QueryMatch query_add;
Net &r = query_add.sig_in("a", hw_int32) + query_add.sig_in("b", hw_int32);
// if the result is not marked as output the query searches only
// the unconnected results as "r" is not connected anywhere 
r.direction = Direction::DIR_OUT;
// match_t is map<object from query*, object from ctx*>
std::vector<QueryMatch::match_t> qres = query_add.search(ctx);
```

# Examples

Currently the examples are parts of the tests. It is better in this phase of the developement as the examples are always verified to be working and actual.

# Similar libraries

* [chisel3.FIRRTL](https://github.com/freechipsproject/firrtl) Hardware circuit description language for Chisel3
* [Yosys.RTLIL](https://github.com/YosysHQ/yosys) Verilog AST like IR in Yosys
* [thorin](https://github.com/AnyDSL/thorin) The Higher-Order Intermediate Representation for LLVM
* [lgraph.core](https://github.com/masc-ucsc/lgraph/tree/master/core) Graph centric library for circuit developement
* [mockturtle](https://github.com/lsils/mockturtle) - logic network library
* [percy](https://github.com/whaaswijk/percy) - Collection of different synthesizers and exact synthesis methods for use in applications such as circuit resynthesis and design exploration.
* [CoSA](https://github.com/cristian-mattarei/CoSA) - CoreIR Symbolic Analyzer
