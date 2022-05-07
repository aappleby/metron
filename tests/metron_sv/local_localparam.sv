`include "metron_tools.sv"

module Module
(
  input logic clock,
  output logic[7:0] tock_ret
);
/*public:*/

  function logic[7:0] tock();
    parameter thing1 = 1;
    parameter thing2 = 2;
    tock = thing1 + thing2;
  endfunction

  always_comb begin
    tock_ret = tock();
  end


endmodule
