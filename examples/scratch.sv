`include "metron/tools/metron_tools.sv"

module Submod (
  // global clock
  input logic clock
);
/*public:*/

  always_ff @(posedge clock) begin : tick
    y <= y + 1;
  end

/*private:*/

  int y;
endmodule

module Module (
  // global clock
  input logic clock
);
/*public:*/

  always_comb begin : tock
    x = 1;
    /*s.tick();*/
  end

/*private:*/
  int x;
  Submod s(
    // Global clock
    .clock(clock)
  );
endmodule
