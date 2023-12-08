`include "metron/metron_tools.sv"

// If statements whose sub-blocks contain submodule calls _must_ use {}.

module Submod (
  // global clock
  input logic clock,
  // tock() ports
  input logic[7:0] tock_arg
);
/*public:*/
  always_ff @(posedge clock) begin : tock
    tick(tock_arg);
  end
/*private:*/
  task automatic tick(logic[7:0] arg);
    my_reg_ <= my_reg_ + arg;
  endtask

  logic[7:0] my_reg_;
endmodule


module Module (
  // global clock
  input logic clock
);
/*public:*/

  always_comb begin : tock
    if (1) begin
      submod__tock_arg = 72;
      /*submod_.tock(72);*/
    end
    else begin
      submod__tock_arg = 36;
      /*submod_.tock(36);*/
    end
  end

  Submod submod_(
    // Global clock
    .clock(clock),
    // tock() ports
    .tock_arg(submod__tock_arg)
  );
  logic[7:0] submod__tock_arg;
endmodule
