`include "metron_tools.sv"

// Tock methods can return values.

module Module
(
  input logic clock,
  output logic[7:0] tock_ret
);
/*public:*/

  function logic[7:0] tock();
    tock = 123;
  endfunction
  //----------------------------------------
  always_comb begin
    tock_ret = tock();
  end
endmodule
