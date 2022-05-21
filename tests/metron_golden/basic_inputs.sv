`include "metron_tools.sv"

// Tick inputs should become input ports.

// clang-format off
module Module
(
  // global clock
  input logic clock,
  // tock() bindings
  input logic[6:0] tock_my_input
);
 /*public:*/

  always_comb begin : tock
    tick_my_input = tock_my_input;
  end

 /*private:*/

  always_ff @(posedge clock) begin : tick
    my_reg <= my_reg + tick_my_input;
  end
  logic[6:0] tick_my_input;

  logic[6:0] my_reg;
endmodule
// clang-format on
