`include "metron/metron_tools.sv"

// Increment/decrement should be translated into equivalent Verilog, but they
// do _not_ return the old/new value.

module Module
(
  // global clock
  input logic clock,
  // output registers
  output int my_reg1_,
  output int my_reg2_,
  output int my_reg3_,
  output int my_reg4_
);
/*public:*/

  always_ff @(posedge clock) begin : tick
    my_reg1_ <= my_reg1_ + 1;
    my_reg2_ <= my_reg2_ + 1;
    my_reg3_ <= my_reg3_ - 1;
    my_reg4_ <= my_reg4_ - 1;
  end

endmodule
