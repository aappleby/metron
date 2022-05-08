`include "metron_tools.sv"

// If statements whose sub-blocks contain submodule calls _must_ use {}.

module Submod
(
  input logic clock,
  input logic[7:0] tock_arg
);
/*public:*/
  function tock(logic[7:0] arg);
    tick_arg = arg;
  endfunction
  always_comb tock(tock_arg);
/*private:*/
  task automatic tick(logic[7:0] arg);
    my_reg <= my_reg + arg;
  endtask
  logic[7:0] tick_arg;
  always_ff @(posedge clock) tick(tick_arg);

  logic[7:0] my_reg;
endmodule


module Module
(
  input logic clock
);
/*public:*/

  function tock();
    if (1) begin
      submod_tock_arg = 72;
    end
    else begin
      submod_tock_arg = 36;
    end
  endfunction
  always_comb tock();

  Submod submod(
    .clock(clock),
    .tock_arg(submod_tock_arg)
  );
  logic[7:0] submod_tock_arg;

endmodule
