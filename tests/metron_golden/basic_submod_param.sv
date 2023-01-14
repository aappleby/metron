`include "metron_tools.sv"

// We can instantiated templated classes as submodules.

module Submod
#(
parameter int SOME_CONSTANT = 6
)
(
  // global clock
  input logic clock
);
/*public:*/

  always_comb begin : tock
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
  end

  Submod #(99) submod(
    // global clock
    .clock(clock)
  );

endmodule
