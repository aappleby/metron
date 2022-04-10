`include "metron_tools.sv"

// Public signal member variables get moved to the output port list.

module Module
(
  input logic clock,
  output logic my_sig
);

  always_comb begin /*tock*/
    my_sig = 1;
  end

endmodule

