`include "metron_tools.sv"

// Modules can contain other modules.

module Submod
(
  input logic clock
);
/*public:*/

  function void tock();
    /*tick()*/;
  endfunction

/*private:*/

  function void tick();
    sub_reg <= sub_reg + 1;
  endfunction

  logic[7:0] sub_reg;

  //----------------------------------------

  always_comb tock();
  always_ff @(posedge clock) tick();

endmodule

module Module
(
  input logic clock
);
/*public:*/

  function void tock();
    /*submod.tock*/;
  endfunction

  Submod submod(
    .clock(clock)
  );

  //----------------------------------------

  always_comb tock();


endmodule
