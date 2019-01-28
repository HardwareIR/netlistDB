# NetlistDB

NetlistDB is an experimental netlist database for hardware developement tools like hardware synthesis tools, simulators and code generators.


# Circuit representation in NetlistDB

|              | Verilog | VHDL                  | NetlistDB |
|:-------------|---------|-----------------------|-----------|
| net(Signal)  | wire    | signal                | Net       |
| component    | module  | entity + architecture | Netlist   |


