`include "metron_tools.sv"

module Module
(
  input logic clock,
  output logic[7:0] tock_ret
);
/*public:*/

  function logic[7:0] tock();
    parameter int thing1 = 1;
    parameter int thing2 = 2;
    tock = thing1 + thing2;
  endfuction
  always_comb tock_ret = tock();

endmodule;

