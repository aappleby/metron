Preface:
  C++ and Verilog are languages that at a glance have a lot of similarities, but have very different semantics in how they execute. To explain how Metron works I'm going to have to take some liberties with terminology and oversimplify some details a bit, as we're effectively writing in two languages at once and trying to completely explain the "why"s all at once would bog things down.

  Restrictions that may seem arbitrary and weird to a C++ programmer exist because we're forced to code in a subset of C++ that can be translated to Verilog, and the semantics that Verilog places on its programs have to be represented back in C++ somehow.

  I've tried to keep things on the C++ side as normal-looking as possible, so those restrictions are represented more in "you have to structure your code this way" rules instead of "you have to annotate your code with these markers and prefix your variable names this way" rules.

To help the explanations make sense, we'll have to switch between a couple different viewpoints when discussing the examples:

- The C++ viewpoint, where we interpret each line of code as happening sequentially and our code blocks are connected by calls and returns.

- The Verilog viewpoint, where blocks of code are "triggered" to evaluate when their inputs (or the clock signal) changes and multiple blocks of code can be triggered and reevaluated simultaneously.

- The hardware viewpoint, where our "code" is actually a description of hundreds (or millions) of logic gates ganged together to form circuits that can do things like add numbers or move bits around and our "variables" are really just names for the bundles of wires that connect our circuit elements together.




So, you want to write some Verilog in C++? Verilog tutorials always seem to start with an adder, so here's Metron's equivalent:

//------------------------------------------------------------------------------

class Adder {
public:
  int add(int x, int y) {
    return x + y;
  }
}

//------------------------------------------------------------------------------

Pretty trivial, eh? Running that through Metron will produce the following SystemVerilog code:

//------------------------------------------------------------------------------

module Adder
(
  input logic clock,
  input int add_x,
  input int add_y,
  output int add
);
/*public:*/
  always_comb begin /*add*/
    add = add_x + add_y;
  end
endmodule

//------------------------------------------------------------------------------

The syntax is different, but you should be able to tell what's going on here - "class" turns into "module", function arguments turn into input ports, function return values turn into output ports.

The "clock" input here is unused and will be explained later.

Let's add another module to make use of the adder.

//------------------------------------------------------------------------------

class Testbench {
public:
  int add_one_plus_two() {
    return adder.add(1, 2);
  }

private:
  Adder adder;
};

//------------------------------------------------------------------------------

module Testbench
(
  input logic clock,
  output int add_one_plus_two
);
/*public:*/
  always_comb begin /*add_one_plus_two*/
    adder_add_x = 1;
    adder_add_y = 2;
    add_one_plus_two = adder_add;
  end

/*private:*/
  Adder adder(
    // Inputs
    .clock(clock),
    .add_x(adder_add_x),
    .add_y(adder_add_y),
    // Outputs
    .add(adder_add)
  );
  int adder_add_x;
  int adder_add_y;
  int adder_add;

endmodule

//------------------------------------------------------------------------------

Whoah, that got much bigger than the previous example. What's going on here? Well, to understand what the extra lines do we need to talk about ports.

In Verilog, there's no notion of one object "calling" another - there's no call stack to put the adder's arguments on, no "call" instruction to execute. Instead, we have ports. There's no exact equivalent in C++ to ports, but you can think of them vaguely like references -

int x = 1;
int& y = x;
y = 2;

To represent something like a C++ call in Verilog, we have to do a couple steps -

1. Define a module that will represent the "callee".
2. Add input ports to that module that will receive the call arguments.
3. Define an output port that will receive the return value of the call.
4. Define a module that will represent the "caller"
5. Define module-level variables* in the caller that we can "bind" to the ports.
6. Bind the caller's variables to the callee's ports.
7. Add some code to the caller that will set those variables.
*Not technically variables, I'm abusing terminology again.

In the C++ viewpoint, our testbench puts the arguments 1 and 2 on the stack and calls adder.add(). I'm going to assume that you the reader are already familiar with how function calls work, so I won't go into that further.

In the Verilog viewpoint, our testbench writes* 1 and 2 to the module-level variables "adder_add_x" and "adder_add_y". Those variables are connected to ports "add_x" and "add_y" on our Adder module, and that in turn triggers the code in Adder to be re-evaluated, which then reads the other end of the ports and writes the sum to the "add" output port, which then goes back to "adder_add", which triggers Testbench to re-evaluate, which then assigns the result "3" to "add_one_plus_two". Whew.

*Not technically "write", but again I have to abuse some terminology.

In the hardware viewpoint, we have a bundle of 32 wires (because 32-bit integer) named "adder_add_x" permanently wired to 0b000..01 on one end and plugged into one input of a 32-bit adder circuit on the other end. There's another bundle of wires named "adder_add_y" carrying 0b000..10 plugged into the other input of the adder circuit, and a third bundle of wires coming out of the adder labeled "add_one_plus_two". The details of what's actually "inside" the adder represented by the "+" symbol in hardware-world is a bit out of scope for this document, but it's effectively hundreds of transistors forming dozens of logic gates that collectively are able to add two 32-bit integers. In FPGA-world it's more like dozens of LUTs (tiny bit-level lookup tables) connected by "programmable wires", but the effect is roughly the same.

The thing that's going to hurt your brain at first (and the thing that's going to make hardware devs furrow their eyebrows) is that all three of these viewpoints are functionally equivalent.

Not that C++ and Verilog are equivalent in the general case - you can easily define code that makes sense in one language but not the other. Recursive calls in C have no equivalent in Verilog, and combinational logic feedback loops have no real equivalent in C. The point of Metron is to restrict your C++ code to a subset that _does_ make sense in both languages.



Let's look at another simple module, this time with some state: a basic counter.

class Counter {
public:

  int get_counter() {
    return counter;
  }

  void increment_counter() {
    counter = counter + 1;
  }

private:

  int counter;

};


All top-level methods in the module (those not called by other methods) become "tick" and "tock" functions.

