`include "metron_tools.sv"

// Port and function arg names can collide, the latter is disambiguated by its
// function name.

module Module
(
  input logic clock,
  input logic[2:0] input_val,
  output logic[2:0] output_val,
  input logic[2:0] tock_input_val,
  output logic[2:0] tock
);
/*public:*/


  always_comb begin /*tock1*/
    output_val = input_val + 7;
  end

  always_comb begin /*tock*/
    tock = tock_input_val + 8;
  end
endmodule

