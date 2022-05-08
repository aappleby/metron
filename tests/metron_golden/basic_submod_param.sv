`include "metron_tools.sv"

// We can instantiated templated classes as submodules.

module Submod
#(parameter int SOME_CONSTANT = 6)
(
  input logic clock
);
/*public:*/

  function tock();
  endfunction
  always_comb tock();

/*private:*/

  task automatic tick();
    sub_reg <= sub_reg + SOME_CONSTANT;
  endtask
  always_ff @(posedge clock) tick();

  logic[7:0] sub_reg;
endmodule

module Module
(
  input logic clock
);
/*public:*/

  function tock();
  endfunction
  always_comb tock();

  Submod #(99) submod(
    .clock(clock)
  );

endmodule
