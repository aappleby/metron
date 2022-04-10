`include "metron_tools.sv"

// Tock methods can return values.

module Module
(
  input logic clock,
  output logic[7:0] tock
);

  always_comb begin /*tock*/
    tock = 123;
  end
endmodule

