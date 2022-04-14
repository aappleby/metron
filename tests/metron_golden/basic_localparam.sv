`include "metron_tools.sv"

// Static const members become SV localparams

module Module
(
  input logic clock,
  output logic[6:0] my_reg
);
/*public:*/

  localparam int my_val = 7;

  always_ff @(posedge clock) begin /*tick*/
    my_reg <= my_val;
  end

endmodule

