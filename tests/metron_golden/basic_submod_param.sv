`include "metron/metron_tools.sv"

// We can instantiated templated classes as submodules.

module Submod (
  // global clock
  input logic clock
);
  parameter SOME_CONSTANT = 6;
/*public:*/

  always_ff @(posedge clock) begin : tock
    tick();
  end

/*private:*/

  task automatic tick();
    sub_reg_ <= sub_reg_ + SOME_CONSTANT;
  endtask

  logic[7:0] sub_reg_;
endmodule

module Module (
  // global clock
  input logic clock
);
/*public:*/

  always_comb begin : tock
    /*submod.tock();*/
  end

  Submod #(
    // Template Parameters
    .SOME_CONSTANT(99)
  ) submod(
    // Global clock
    .clock(clock)
  );
endmodule
