`include "metron_tools.sv"

// Trivial adder just for example.

module Adder
(
  input logic clock,
  input logic[7:0] add_a,
  input logic[7:0] add_b,
  output logic[7:0] add_ret
);
/*public:*/

  function logic[7:0] add(logic[7:0] a, logic[7:0] b);
    add = a + b;
  endfunction

  always_comb begin
    add_ret = add(add_a, add_b);
  end


  always_ff @(posedge clock) begin
  end


endmodule
