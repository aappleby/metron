`include "metron/tools/metron_tools.sv"

// Empty module should pass.

module Module (
  // global clock
  input logic clock
);
  logic x;
  logic y;

  always_ff @(posedge clock) begin : tick
    x <= ~y;
    y <= x;
  end
endmodule
