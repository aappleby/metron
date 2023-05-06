`include "metron/tools/metron_tools.sv"

// Modules can contain other modules.

module Submod  (
  // global clock
  input logic clock
);
/*public*/

  always_comb begin : tock
  end

/*private*/

  always_ff @(posedge clock) begin : tick
    sub_reg <= sub_reg + 1;
  end

  logic[7:0] sub_reg;
endmodule

module Module  (
  // global clock
  input logic clock
);
/*public*/

  always_comb begin : tock
  end

  Submod submod(
    // Global clock
    .clock(clock)
  );
endmodule
