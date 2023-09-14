`include "metron/tools/metron_tools.sv"

// Number literals
// don't forget the ' spacers

module Module (
  // test1() ports
  output int test1_ret,
  // test2() ports
  output int test2_ret
);
/*public:*/

  always_comb begin : test1
    a=0b0;
    b=0b00;
    c=0b000;
    d=0b0000;
    e=0b00000;
    f=0b000000;
    g=0b0000000;
    h=0b00000000;
    test1_ret = 0;
  end

  always_comb begin : test2
    a=0b0;
    b=0b0'0;
    c=0b0'00;
    d=0b00'00;
    e=0b00'000;
    f=0b0'000'00;
    g=0b000'0000;
    h=0b0'0'0'0'0'0'0'0;
    test2_ret = 0;
  end

endmodule;