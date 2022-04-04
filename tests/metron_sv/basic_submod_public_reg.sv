`include "metron_tools.sv"

// We can instantiated templated classes as submodules.

module Submod
(
  input logic clock,
  output logic[7:0] sub_reg
);
/*public:*/

  task tick(); 
    sub_reg <= sub_reg + 1;
  endtask
  always_ff @(posedge clock) tick();

  /*logic<8> sub_reg;*/
endmodule

module Module
(
  input logic clock,
  output logic[7:0] get_submod_reg
);
/*public:*/

  always_comb begin
    get_submod_reg = submod_sub_reg;
  end

  always_comb begin /*tock*/
    /*submod.tick();*/
  end

  Submod submod(
    // Inputs
    .clock(clock),
    // Outputs
    .sub_reg(submod_sub_reg)
  );
  logic[7:0] submod_sub_reg;

endmodule

