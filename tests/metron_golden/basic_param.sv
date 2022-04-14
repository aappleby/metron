`include "metron_tools.sv"

// Template parameters become SV module parameters.

module Module
#(parameter int SOME_CONSTANT = 7)
(
  input logic clock,
  output logic[6:0] my_reg
);
/*public:*/

  always_ff @(posedge clock) begin /*tick*/
    my_reg <= SOME_CONSTANT;
  end

endmodule

