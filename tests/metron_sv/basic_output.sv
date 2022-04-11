`include "metron_tools.sv"

// Getter methods should turn into outputs.

module Module
(
  input logic clock,
  output logic[6:0] my_reg,
  output logic[6:0] get_reg
);
/*public:*/

  always_comb begin
    get_reg = my_reg;
  end

  always_ff @(posedge clock) begin : tick
    my_reg <= my_reg + 1;
  end

endmodule

