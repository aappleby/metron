`include "metron_tools.sv"

// Cramming various statements into one line should not break anything.

module Module
(
  input logic clock,
  output logic[7:0] tock2
);
/*public:*/

  always_comb begin /*tock1*/ /*tick()*/; end
  always_comb begin /*tock2*/ logic[7:0] a;

a = 1; a = a + 7; tock2 = a; end

/*private:*/

  always_ff @(posedge clock) begin /*tick*/ if (my_reg & 1) my_reg <= my_reg - 7; end

  logic[7:0] my_reg;

endmodule

