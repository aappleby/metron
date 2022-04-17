`include "metron_tools.sv"

// Trivial adder just for example.

module Adder
(
  input logic clock,
  input logic[7:0] add_a,
  input logic[7:0] add_b,
  output logic[7:0] add
);
/*public:*/

  always_comb begin /*add*/
    add = add_a + add_b;
  end

endmodule

