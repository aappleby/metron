`include "metron/tools/metron_tools.sv"

// DONTCARE gets translated to 'x
// bN(DONTCARE) gets translated to N'bx

module Module (
  // test1() ports
  output logic[7:0] test1_ret,
  // test2() ports
  output logic[7:0] test2_ret,
  // test3() ports
  output logic[7:0] test3_ret
);
/*public:*/

  always_comb begin : test1
    test1_ret = 'x;
  end

  always_comb begin : test2
    test2_ret = 8'bx;
  end

  always_comb begin : test3
    test3_ret = 8'('x);
  end
endmodule
