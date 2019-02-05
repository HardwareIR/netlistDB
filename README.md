# NetlistDB

NetlistDB is an experimental netlist database for hardware developement tools like hardware synthesis tools, simulators and code generators.

It is designed to allow efficient parallel processing of complex task while not sacrifising single thread performance.


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

[1] The Assignment object constructed by call operator on Net is non blocking as blocking assingment can be used the c++ assignment.
[2] Each statement is self sufficient process with automatically managed sensitivity. (There is transformation class which transforms
    the statements to readable format so the serializae output is minimalized and readable.)
    


# Structure of NetlistDB

The main object in NetlistDB is netlistDB::Netlist this object is a container of Net objects.
This object can be used to build the circuits as it is shown in example below.

```cpp
Netlist ctx("adder");
Net &a = ctx.sig_in("a");
Net &b = ctx.sig_in("b");
Net &res_tmp = a + b;
Net &res = ctx.sig_out("out");
res(res_tmp);
```

To perform analysis of the Netlist there is netlistDB::query namespace.
The netlistDB::query::QueryMatch can be used to perform graph pattern match (isomorphic graph search).
The QueryMatch is also Netlist. The query is specified by the other circuit as shown in example below.

```cpp
QueryMatch query_add;
Net &r = query_add.sig_in("a") + query_add.sig_in("b");
// if the result is not marked as output the query searches only
// the unconnected results as "r" is not connected anywhere 
r.direction = Direction::DIR_OUT;
// match_t is map<object from query*, object from ctx*>
std::vector<QueryMatch::match_t> qres = query_add.search(ctx);
```

