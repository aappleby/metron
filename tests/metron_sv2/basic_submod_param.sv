`include "metron/tools/metron_tools.sv"

// We can instantiated templated classes as submodules.

module Submod (
  // global clock
  input logic clock,
  // output registers
  output logic[7:0] sub_reg
);
  parameter SOME_CONSTANT = 6;

/*public:*/

  always_comb begin : tock
    /*tick();*/
  end

/*private:*/

  always_ff @(posedge clock) begin : tick
    sub_reg <= sub_reg + SOME_CONSTANT;
  end

  logic[7:0] sub_reg;
endmodule

module Module (
  // global clock
  input logic clock
);
/*public:*/

  always_comb begin : tock
    /*submod.tock();*/
  end

  Submod #(
    // Template Parameters
    .SOME_CONSTANT(99)
  ) submod(
    // Global clock
    .clock(clock),
    // Output registers
    .sub_reg(submod_sub_reg)
  );
endmodule
