`include "metron/metron_tools.sv"

// Comments surrounded by / * # <something # * / get unwrapped and dropped
// directly in the output file.

module Module
(
  // global clock
  input logic clock,
  // output registers
  output int my_reg_
);
/*public:*/

  always_ff @(posedge clock) begin : tick
    my_reg_ <= my_reg_ + 1;
  end


  always @(posedge clock) begin
    //$display("Hello World!\n");
  end


endmodule
