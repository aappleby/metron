`include "metron_tools.sv"

// If statements whose sub-blocks contain submodule calls _must_ use {}.

module Submod
(
  input logic clock,
  input logic[7:0] tock_arg
);
/*public:*/
  always_ff @(posedge clock) begin /*tock*/
tick_arg = tock_arg;
        /*tick(arg)*/;
  end
/*private:*/
  logic[7:0] tick_arg;
  always_ff @(posedge clock) begin /*tick*/
    my_reg <= my_reg + arg;
  end

  logic[7:0] my_reg;
endmodule


module Module
(
  input logic clock
);
/*public:*/

  always_ff @(posedge clock) begin /*tock*/
    if (1) begin
submod_tock_arg = 72;
            /*submod.tock(72)*/;
    end
    else begin
submod_tock_arg = 36;
            /*submod.tock(36)*/;
    end
  end

  Submod submod(
    // Inputs
    .clock(clock),
    .tock_arg(submod_tock_arg)
    // Outputs
  );
  logic[7:0] submod_tock_arg;

endmodule

