`include "metron_tools.sv"

// We can instantiated templated classes as submodules.

module Submod
#(parameter int SOME_CONSTANT = 6)
(
  input logic clock
);
/*public:*/

  function void tock();
    /*tick()*/;
  endfunction

/*private:*/

  function void tick();
    sub_reg <= sub_reg + SOME_CONSTANT;
  endfunction

  logic[7:0] sub_reg;

  //----------------------------------------
  always_comb begin
    tock();
  end


  always_ff @(posedge clock) begin
    tick();
  end

endmodule

module Module
(
  input logic clock
);
/*public:*/

  function void tock();
    /*submod.tock*/;
  endfunction

  Submod #(99) submod(
    .clock(clock)
  );

  //----------------------------------------
  always_comb begin
    tock();
  end


endmodule
