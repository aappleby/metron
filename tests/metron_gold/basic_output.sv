`include "metron_tools.sv"

// Getter methods should turn into outputs.

module Module
(
  input logic clock,
  output logic[6:0] my_reg,
  output logic[6:0] get_reg
);

  always_comb begin
    get_reg = my_reg;
  end

  task tick();
    my_reg <= my_reg + 1;
  endtask
  always_ff @(posedge clock) tick();

endmodule

