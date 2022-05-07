`include "metron_tools.sv"

// DONTCARE gets translated to 1'bx

module Module
(
  input logic clock,
  output logic[7:0] tock2_ret,
  output logic[7:0] tock3_ret
);
/*public:*/

  //logic<8> tock1() {
  //  return DONTCARE;
  //}

  function logic[7:0] tock2();
    tock2 = 8'bx;
  endfunction

  function logic[7:0] tock3();
    tock3 = 8'(1'bx);
  endfunction

  always_comb begin
    tock2_ret = tock2();
    tock3_ret = tock3();
  end


  always_ff @(posedge clock) begin
  end

endmodule
