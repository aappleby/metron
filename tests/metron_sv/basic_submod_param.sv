`include "metron_tools.sv"

// We can instantiated templated classes as submodules.

module Submod
#(parameter int SOME_CONSTANT = 6)
(
  input logic clock
);
/*public:*/

  always_comb begin /*tock*/
    /*tick()*/;
  end

/*private:*/

  always_ff @(posedge clock) begin /*tick*/
    sub_reg <= sub_reg + SOME_CONSTANT;
  end

  logic[7:0] sub_reg;
endmodule

module Module
(
  input logic clock
);
/*public:*/

  always_comb begin /*tock*/
    /*submod.tock()*/;
  end

  Submod #(99) submod(
    // Inputs
    .clock(clock)
    // Outputs
  );

endmodule
