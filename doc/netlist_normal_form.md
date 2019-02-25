# The netlist normal form
The netlist normal form is simplified format of netlist where each process can be directly mapped to some type of component.

One of the normal forms is mentioned in this publication:
http://www.academia.edu/12322605/Symbolic_verification_of_synthesized_RTL_using_Boolean_satisfiability_and_uninterpreted_RTL_transformations

For now I am using custom definition below.

The circuit is in normal form if each statement is primitive assignemt, demultiplexer, multiplexer, register, memory port, latch or latch with non separable mux.
The circuit does not contain ternary operator.
The assignment is primitive if destination does not contain any index operator.
The demultiplexer is assignment where source is indexed by non constant signal.
The multiplexer is a "switch" or "if" statement where all code branches are covered.
The register and memory port have exactly one clock signal and up to one clock enable signal.
The register can have up to one asynchronous reset. (= async reset "if" can contain only primitive assignment.)
	```
	if (async_rst)
	   reg = preset; // only simple assignment
	else if (clk.risig()) {
	   reg = reg_next; // only simple assignment
	}
```

Each ram write port can be only in formats below.
```
	  if (clk and en) { // en is optional
	  	 mem[addr] = input;
	  }
```
```  
	if (clk and en) { // en is optional
	   if (range_x_en) {  // repeated n times
		     mem[addr][const_range_x] = input[const_range_x];
	   }
	}
```
Each ram read port can be only in format below.
```
	if (clk and en) { // en is optional
	   output = mem[addr];
	}
```
Switch or "if" statement is latch if it has some branches unused.
   * If the latch statement contains multiplexier statement the multiplexier statement is separable.
   * The non separable multiplexier is the "multiplexier" in the latch statement it self.
```
	// latch + mux
	if (a) {
		If (b) {
			b = c;
		} else {
			b = d;
		}
	}
```
   The multiplexer can be separated as:
```
	// pure latch
	if (a) {
		b = b_tmp;
	}
	// pure mux
	If (b) {
		b_tmp = c;
	} else {
		b_tmp = d;
	}
```
   In this example it is not efficient to extract the multiplexer from the latch.
   (Because it would make the processing even more complicated.)
```
if (a or b)
    c = 0;
else if (a)
    c = 1;
```	

## Why normal form for netlist
The Three-address code is widely used to represent code in software compilers. Without it the analysis of code would be much more complicated.
However just Threee-address code for representation of hardware is not enought. And the netlist itself can descritebe same thing in many ways.
This means that the usual hand-writen code is hard to analyze because many possibilites has to be checked (which makes the check infeasible...).


## The goal
The goal is to have each process representing a single hardware component e.g. multiplexer, latch or FF register.

The format roughtly matches (in the wildcard part the previous elemet has to differ from actual) the "regular expression" below:
```
(reg/ram/latch)-->(mux|demux|expr)*--|
          ^--------------------------|
```


## Conversion of netlist to normal form
1. Remove dead code.
  * The code is dead if it does not affect the output. Or if it is under codition which is canstantly disabled. 

2. Extract all special IO
  1. Extract all tristate logic from all processes
    * for each signal instantiate tristate buffer and use t,i,o signals in original process instead

3. Fill all unused code branches with varialbe preset if present.
```
   if (b) {
   	  a = 0;
   }
   If (c) {
   	  a = 1;
   }
```
	to
```
   If (c) {
   	  a = 1;
   } else if (b) {
   	  a = 0;
   }

```
4. Separate demulteplexers.

5. Simplify the "if"/"switch" statements.
   * If the condition does not have effect replace this stement only with the first branch.
   * The condition does not have effect if all branches are same.

6. Replace all muxes which are representing b = bool(a) or b = ~bool(a) with this assignments.
7. Separate pure latches from the potential multiplexes.
8. Separate pure registers from the registers from potential multiplexer.
	* Rewrite register next logic using register_next signal.

9. Separate ram ports.
10. Merge multiplexiers.
   * If the input is multipexier and it's output is used only as a input of this multiplexier inline it to this multiplexier.



