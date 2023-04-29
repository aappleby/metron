`include "metron_tools.sv"

// Trivial adder just for example.

module Adder (
  // add() ports
  input logic[7:0] add_a,
  input logic[7:0] add_b,
  output logic[7:0] add_ret
);
/*public*/

  always_comb begin : add
    add_ret = add_a + add_b;
  end

endmodule
