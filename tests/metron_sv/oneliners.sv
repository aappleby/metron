`include "metron_tools.sv"

// Cramming various statements into one line should not break anything.

module Module
(
  input logic clock,
  output logic[7:0] tock
);
/*public:*/

  always_comb begin /*tock*/ logic[7:0] a;

a = 1; a = a + 7; tock = a; end
  always_ff @(posedge clock) begin /*tick*/ if (my_reg & 1) my_reg <= my_reg - 7; end

/*private:*/
  logic[7:0] my_reg;

endmodule

