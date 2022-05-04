`include "metron_tools.sv"

// Tick inputs should become input ports.

module Module
(
  input logic clock,
  input logic[6:0] tock_my_input
);
 /*public:*/
  always_ff @(posedge clock) begin /*tock*/ tick_my_input = tock_my_input;
/*tick(my_input)*/; end

 /*private:*/
  logic[6:0] tick_my_input;
  always_ff @(posedge clock) begin /*tick*/ my_reg <= my_reg + tick_my_input; end

  logic[6:0] my_reg;
endmodule

