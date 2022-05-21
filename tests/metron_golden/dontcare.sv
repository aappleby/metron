`include "metron_tools.sv"

// DONTCARE gets translated to 1'bx

module Module
(
  // test2() ports
  output logic[7:0] test2_ret,
  // test3() ports
  output logic[7:0] test3_ret
);
/*public:*/

  // FIXME why is this broken?
  /*
  logic<8> test1() {
    return DONTCARE;
  }
  */

  always_comb begin : test2
    test2_ret = 8'bx;
  end

  always_comb begin : test3
    test3_ret = 8'(1'bx);
  end
endmodule
