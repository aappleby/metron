`include "metron_tools.sv"

// We can instantiated templated classes as submodules.

module Submod
#(parameter int SOME_CONSTANT = 6)
(
  input logic clock,
  output logic[7:0] sub_reg
);
/*public:*/

  always_ff @(posedge clock) begin : tick
    sub_reg <= sub_reg + SOME_CONSTANT;
  end

endmodule

module Module
(
  input logic clock
);
/*public:*/

  always_comb begin /*tock*/
    /*submod.tick()*/;
  end

  Submod #(99) submod(
    // Inputs
    .clock(clock),
    // Outputs
    .sub_reg(submod_sub_reg)
  );
  logic[7:0] submod_sub_reg;

endmodule

