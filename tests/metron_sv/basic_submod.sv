`include "metron_tools.sv"

// Modules can contain other modules.

module Submod
(
  input logic clock
);
/*public:*/

  always_comb begin : tock
  end

/*private:*/

  task automatic tick();
    sub_reg <= sub_reg + 1;
  endtask
  always_ff @(posedge clock) tick();

  logic[7:0] sub_reg;
endmodule

module Module
(
  input logic clock
);
/*public:*/

  always_comb begin : tock
  end

  Submod submod(
    .clock(clock)
  );

endmodule
