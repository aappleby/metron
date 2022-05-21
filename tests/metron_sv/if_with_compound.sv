`include "metron_tools.sv"

// If statements whose sub-blocks contain submodule calls _must_ use {}.

module Submod
(
  // global clock
  input logic clock,
  // tock() ports
  input logic[7:0] tock_arg
);
/*public:*/
  always_comb begin : tock
    tick_arg = tock_arg;
  end
/*private:*/
  always_ff @(posedge clock) begin : tick
    my_reg <= my_reg + tick_arg;
  end
  logic[7:0] tick_arg;

  logic[7:0] my_reg;
endmodule


module Module
(
  // global clock
  input logic clock
);
/*public:*/

  always_comb begin : tock
    if (1) begin
      submod_tock_arg = 72;
    end
    else begin
      submod_tock_arg = 36;
    end
  end

  Submod submod(
    .clock(clock),
    .tock_arg(submod_tock_arg)
  );
  logic[7:0] submod_tock_arg;

endmodule
