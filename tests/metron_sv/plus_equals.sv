module Metron (
  // global clock
  input logic clock,
  // output signals
  output int my_sig,
  // output registers
  output int my_reg1_,
  output int my_reg2_,
  output int my_reg3_,
  output int my_reg4_,
  output int my_reg5_,
  output int my_reg6_
);
/*public:*/


  // Divide and mod work, but make Yosys extremely slow to synth

  always_comb begin : tock
    int x;
    x = 7;
    x = x + 13;
    x  = x - 13;
    x   = x * 13;
    x = x +   13;
    x = x -  13;
    x = x * 13;
    my_sig = x;
  end


  always_ff @(posedge clock) begin : tick
    my_reg1_ <= my_reg1_ + 22;
    my_reg2_  <= my_reg2_ - 22;
    my_reg3_   <= my_reg3_ * 22;
    my_reg4_ <= my_reg4_ +   22;
    my_reg5_ <= my_reg5_ -  22;
    my_reg6_ <= my_reg6_ * 22;
  end
endmodule
