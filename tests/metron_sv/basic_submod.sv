`include "metron_tools.sv"

// Modules can contain other modules.

module Submod
(
  input logic clock
);
/*public:*/

  task  tock();
    /*tick()*/;
  endtask
  always_comb tock();

/*private:*/

  task  tick();
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

  task  tock();
    /*submod.tock*/;
  endtask
  always_comb tock();

  Submod submod(
    .clock(clock)
  );

endmodule
