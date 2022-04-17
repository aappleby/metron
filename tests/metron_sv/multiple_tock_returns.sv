`include "metron_tools.sv"

// Tock functions can't have more than a single return at the end.

module Module
(
  input logic clock,
  input logic[7:0] tock_data,
  output logic[7:0] tock
);
/*public:*/

  always_comb begin /*tock*/
    if (tock_data > 10) begin
      tock = 12;
    end
    tock = 11;
  end
endmodule

