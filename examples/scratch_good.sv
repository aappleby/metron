`include "metron/tools/metron_tools.sv"

// Modules can contain other modules.

module Submod (
  // global clock
  input logic clock,
  // tock() ports
  output int tock_ret
);
/*public*/

  parameter width = 7;
  initial begin
  end

  always_comb begin : tock
    tock_ret = 1;
  end

/*private*/

  always_ff @(posedge clock) begin : tick
    sub_reg <= sub_reg + 1;
  end

  logic[7:0] sub_reg;
endmodule

module Module (
  // global clock
  input logic clock
);
/*public*/

  initial begin
  end

  always_comb begin : tock
  end

  Submod #(
    // Constructor Parameters
    .width(2)
  ) submod(
    // Global clock
    .clock(clock),
    // tock() ports
    .tock_ret(submod_tock_ret)
  );
  int submod_tock_ret;
endmodule
