`include "metron_tools.sv"

// Modules can contain other modules.

module Submod
(
  input logic clock,
  output logic[7:0] sub_reg
);
/*public:*/

  always_ff @(posedge clock) begin : tick
    sub_reg <= sub_reg + 1;
  end

endmodule

module Module
(
  input logic clock
);
/*public:*/

  always_ff @(posedge clock) begin : tick
    /*submod.tick()*/;
  end

  Submod submod(
    // Inputs
    .clock(clock),
    // Outputs
    .sub_reg(submod_sub_reg)
  );
  logic[7:0] submod_sub_reg;

endmodule

