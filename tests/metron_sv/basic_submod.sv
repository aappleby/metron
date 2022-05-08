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
  always_comb tock();

/*private:*/

  function void tick();
    sub_reg <= sub_reg + 1;
  endfunction
  always_ff @(posedge clock) tick();

  logic[7:0] sub_reg;
endmodule

module Module
(
  input logic clock
);
/*public:*/

  function void tock();
    /*submod.tock*/;
  endfunction
  always_comb tock();

  Submod submod(
    .clock(clock)
  );

endmodule
