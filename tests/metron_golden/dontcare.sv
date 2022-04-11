`include "metron_tools.sv"

// DONTCARE gets translated to 1'bx

module Module
(
  input logic clock,
  output logic[7:0] tock2,
  output logic[7:0] tock3
);
/*public:*/

  //logic<8> tock1() {
  //  return DONTCARE;
  //}

  always_comb begin /*tock2*/
    tock2 = 8'bx;
  end

  always_comb begin /*tock3*/
    tock3 = 8'(1'bx);
  end
endmodule

