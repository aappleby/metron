`include  "metron_tools.sv"

// Tock methods can return values.

module Module
(
  input logic clock,
  output logic[7:0]tock_ret
);
/*public:*/

  function logic[7:0] tock();
    tock = 123;
  endfuction
  always_comb tock_ret = tock();
endmodule;


