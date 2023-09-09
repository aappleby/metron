`include "metron/tools/metron_tools.sv"

module Module (
  // global clock
  input logic clock,
  // tock() ports
  input logic tock_q,
  output logic[2:0] tock_ret
);
/*public*/

  always_comb begin : tock
    out = x + y + z + tock_q;
    tock_ret = 7;
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
