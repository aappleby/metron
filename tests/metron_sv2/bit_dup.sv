`include "metron/tools/metron_tools.sv"

module Module (
  // test_dup1() ports
  output int test_dup1_ret,
  // test_dup4() ports
  output int test_dup4_ret
);
/*public:*/

  always_comb begin : test_dup1
    a1=0b1;
    b1={1 {a1}};
    test_dup1_ret = 0;
  end

  always_comb begin : test_dup4
    a1=0b1;
    b1={4 {a1}};

    a2=0b01;
    b2={4 {a2}};

    a3=0b001;
    b3={4 {a3}};

    a4=0b0001;
    b4={4 {a4}};

    a5=0b00001;
    b5={4 {a5}};

    a6=0b000001;
    b6={4 {a6}};

    a7=0b0000001;
    b7={4 {a7}};

    a8=0b00000001;
    b8={4 {a8}};
    test_dup4_ret = 0;
  end
endmodule