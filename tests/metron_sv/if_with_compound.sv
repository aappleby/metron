`include "metron_tools.sv"

// If statements whose sub-blocks contain submodule calls _must_ use {}.

module Submod
(
  input logic clock,
  input logic[7:0] tock_arg
);
/*public:*/
  function void tock(logic[7:0] arg);
    tick_arg = arg;
    /*tick(arg)*/;
  endfunction
/*private:*/
  function void tick(logic[7:0] arg);
    my_reg <= my_reg + arg;
  endfunction

  logic[7:0] my_reg;

  //----------------------------------------
  always_comb begin
    tock(tock_arg);
  end

  logic[7:0] tick_arg;

  always_ff @(posedge clock) begin
    tick(tick_arg);
  end

endmodule


module Module
(
  input logic clock
);
/*public:*/

  function void tock();
    if (1) begin
      submod_tock_arg = 72;
      /*submod.tock*/;
    end
    else begin
      submod_tock_arg = 36;
      /*submod.tock*/;
    end
  endfunction

  Submod submod(
    .clock(clock),
    .tock_arg(submod_tock_arg)
  );
  logic[7:0] submod_tock_arg;


  //----------------------------------------
  always_comb begin
    tock();
  end

endmodule
