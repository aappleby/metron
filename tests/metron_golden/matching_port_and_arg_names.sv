`include "metron_tools.sv"

// Port and function arg names can collide, the latter is disambiguated by its
// function name.

module Module
(
  input int input_val,
  output int output1,
  output int output2,
  // tock2()
  input int tock2_input_val
);
/*public:*/


  always_comb begin : tock1
    output1 = input_val + 7;
  end

  always_comb begin : tock2
    output2 = tock2_input_val + 8;
  end
endmodule
