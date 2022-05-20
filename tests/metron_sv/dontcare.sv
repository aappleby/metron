`include "metron_tools.sv"

// DONTCARE gets translated to 1'bx

module Module
(
  output logic[7:0] test2_ret,
  output logic[7:0] test3_ret
);
/*public:*/

  // FIXME why is this broken?
  /*
  logic<8> test1() {
    return DONTCARE;
  }
  */

  function logic[7:0] test2();
    test2 = 8'bx;
  endfunction
  always_comb test2_ret = test2();

  function logic[7:0] test3();
    test3 = 8'(1'bx);
  endfunction
  always_comb test3_ret = test3();
endmodule
