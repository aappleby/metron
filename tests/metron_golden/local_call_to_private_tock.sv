`include "metron_tools.sv"

module Module
(
  input logic clock,
  output logic[7:0] my_sig,
  input logic[7:0] tock1_a,
  output logic[7:0] tock1
);
/*public:*/


  always_comb begin /*tock1*/
    /*tock2(a + 4)*/;
    tock1 = tock3;
  end

/*private:*/

  logic[7:0] tock2_a;
  always_comb begin /*tock2*/
    my_sig = tock2_a + 22;
  end

  logic[7:0] tock3_a;
  logic[7:0] tock3;
  always_comb begin /*tock3*/
    tock3 = tock3_a + 13;
  end


endmodule

