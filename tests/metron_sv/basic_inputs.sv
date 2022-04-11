`include "metron_tools.sv"

// Tick inputs should become input ports.

module Module
(
  input logic clock,
  output logic[6:0] my_reg,
  input logic[6:0] tick_my_input
);
/*public:*/

  always_ff @(posedge clock) begin : tick
    my_reg <= my_reg + tick_my_input;
  end

endmodule

