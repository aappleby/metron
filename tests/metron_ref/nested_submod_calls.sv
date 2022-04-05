`include "metron_tools.sv"

// Nesting submod calls should put all the nested input bindings above the call.

module Submod1
(
  input logic clock,
  input logic[7:0] a,
  output logic[7:0] add_one
);
/*public:*/
  always_comb begin
    add_one = a + 1;
  end
endmodule

module Submod2
(
  input logic clock,
  input logic[7:0] a,
  output logic[7:0] add_two
);
/*public:*/
  always_comb begin
    add_two = a + 1;
  end
endmodule

module Module
(
  input logic clock
);
/*public:*/

  task tick(); 
    // Two bindings should end up here.
    submod2_a = counter;
    submod1_a = submod2_add_two;
    counter <= submod1_add_one;
  endtask
  always_ff @(posedge clock) tick();

/*private:*/

  Submod1 submod1(
    // Inputs
    .clock(clock),
    .a(submod1_a), 
    // Outputs
    .add_one(submod1_add_one)
  );
  logic[7:0] submod1_a;
  logic[7:0] submod1_add_one;

  Submod2 submod2(
    // Inputs
    .clock(clock),
    .a(submod2_a), 
    // Outputs
    .add_two(submod2_add_two)
  );
  logic[7:0] submod2_a;
  logic[7:0] submod2_add_two;

  logic[7:0] counter;
endmodule

