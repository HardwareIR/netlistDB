# NetlistDB

[![Build Status](https://travis-ci.org/HardwareIR/hardwareIr.svg?branch=master)](https://travis-ci.org/HardwareIR/hardwareIr)
[![Coverage Status](https://coveralls.io/repos/github/HardwareIR/hardwareIr/badge.svg?branch=master)](https://coveralls.io/github/HardwareIR/hardwareIr?branch=master)

(Note that even the name is not in final version...)

NetlistDB is an experimental netlist database for hardware developement tools (hardware synthesis tools, simulators, code generators, ...).
It is designed to allow efficient parallel processing of complex circut transofmation tasks while not sacrifising single thread performance.

## The purpose of NetlistDB

This library is a backend and intermediate format for representation of digital circuit. The direct use may result in more ugly code than verilog itself. Think of this as blazing fast code generator and the backend for the circuit optimalizers and generators. 

# Circuit representation in NetlistDB


|                         | Verilog   | VHDL                  |  NetlistDB                                    |
|:------------------------|-----------|-----------------------|-----------------------------------------------|
| net(Signal)             | wire      | signal                |  Net object - construt by Netlist.sig()       |
| component               | module    | entity + architecture |  Netlist object                               |
| operatos                | +/-, ...  | +/-, ...              |  FunctionCall object - use overrided Net ops. |
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
sudo apt-get install build-essentials meson cmake git libtbb-dev libboost-all-dev
# [NOTE] if you are using old system like like ubuntu 14 on travis-ci install fresh meson by
#        pip3 install meson instead to avoid complications with libboost discovery
```

build this library
```
meson build # meson is tool similar to cmake or autotools
cd build
ninja # ninja is tool similar to make
```

run tests
```
cd build
ninja test
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

