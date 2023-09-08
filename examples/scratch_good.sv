`include "metron/tools/metron_tools.sv"

module Module (
  // global clock
  input logic clock
);
/*public*/

  always_comb begin : tock
    out = x + y + z;
  end

  always_ff @(posedge clock) begin : tick
    x <= x + 1;
    y <= y + 2;
    z <= z + 1;
  end

/*private*/

  logic x;
  logic y;
  logic z;
  logic[1:0] out;
endmodule
