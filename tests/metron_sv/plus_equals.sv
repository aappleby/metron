module Metron (
  // global clock
  input logic clock,
  // output signals
  output int my_sig,
  output int my_reg1,
  output int my_reg2,
  output int my_reg3,
  output int my_reg4,
  output int my_reg5,
  output int my_reg6
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
    my_reg1 <= my_reg1 + 22;
    my_reg2  <= my_reg2 - 22;
    my_reg3   <= my_reg3 * 22;
    my_reg4 <= my_reg4 +   22;
    my_reg5 <= my_reg5 -  22;
    my_reg6 <= my_reg6 * 22;
  end
endmodule
