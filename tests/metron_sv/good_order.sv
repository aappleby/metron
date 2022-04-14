`include "metron_tools.sv"

// Declaration order _matters_ - a tock() that reads a reg before the tick()
// that writes it is OK.

module Module
(
  input logic clock,
  output logic my_sig,
  output logic my_reg
);
/*public:*/

  always_comb begin /*tock*/
    my_sig = my_reg;
  end

  always_ff @(posedge clock) begin /*tick*/
    my_reg <= 1;
  end

endmodule

