`include "metron_tools.sv"

// Tick inputs should become input ports.

module Module
(
  input logic clock,
  output logic[6:0] my_reg,
  input logic[6:0] my_input
);

  task tick();
    my_reg <= my_reg + my_input;
  endtask
  always_ff @(posedge clock) tick();

endmodule

