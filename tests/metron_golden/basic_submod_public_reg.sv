`include "metron_tools.sv"

// We can instantiated templated classes as submodules.

module Submod
(
  input logic clock,
  output logic[7:0] sub_reg
);
/*public:*/

  always_ff @(posedge clock) begin /*tick*/
    sub_reg <= sub_reg + 1;
  end

endmodule

module Module
(
  input logic clock
);
/*public:*/

  function logic[7:0] get_submod_reg();
    get_submod_reg = submod_sub_reg;
  endfunction

  always_comb begin /*tock*/
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

