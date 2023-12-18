`include "metron/metron_tools.sv"

// Tick inputs should become input ports.

// clang-format off
module Module
(
  // global clock
  input logic clock,
  // tock() ports
  input logic[6:0] tock_my_input
);
 /*public:*/

  always_ff @(posedge clock) begin : tock
    tick(tock_my_input);
  end

 /*private:*/

  task automatic tick(logic[6:0] my_input);
    my_reg_ <= my_reg_ + my_input;
  endtask

  logic[6:0] my_reg_;
endmodule
// clang-format on
