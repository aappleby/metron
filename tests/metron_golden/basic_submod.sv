`include "metron/metron_tools.sv"

// Modules can contain other modules.

module Submod (
  // global clock
  input logic clock,
  // tock() ports
  input int tock_x
);
/*public:*/

  always_ff @(posedge clock) begin : tock
    tick(tock_x);
  end

/*private:*/

  task automatic tick(int x);
    sub_reg_ <= sub_reg_ + x;
  endtask

  logic[7:0] sub_reg_;
endmodule

module Module (
  // global clock
  input logic clock,
  // tock() ports
  input int tock_x
);
/*public:*/

  always_comb begin : tock
    submod_tock_x = tock_x;
    /*submod.tock(x);*/
  end

  Submod submod(
    // Global clock
    .clock(clock),
    // tock() ports
    .tock_x(submod_tock_x)
  );
  int submod_tock_x;
endmodule
