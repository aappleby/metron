`include "metron_tools.sv"

module Module
(
  input logic clock,
  output logic[7:0] tock
);
/*public:*/

  always_comb begin /*tock*/
    localparam int thing1 = 1;
    localparam int thing2 = 2;
    tock = thing1 + thing2;
  end

endmodule

