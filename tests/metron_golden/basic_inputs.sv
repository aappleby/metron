`include "metron_tools.sv"

// Tick inputs should become input ports.

// clang-format off
module Module
(
  input logic clock,
  input logic[6:0] tock_my_input
);
 /*public:*/

  function void tock(logic[6:0] my_input);
    tick_my_input = my_input;
    /*tick(my_input)*/;
  endfunction

 /*private:*/

  function void tick(logic[6:0] my_input);
    my_reg <= my_reg + my_input;
  endfunction

  logic[6:0] my_reg;

  //----------------------------------------

  always_comb tock(tock_my_input);
  logic[6:0] tick_my_input;
  always_ff @(posedge clock) tick(tick_my_input);

endmodule
// clang-format on
