`include "metron/tools/metron_tools.sv"

// Comments surrounded by / * # <something # * / get unwrapped and dropped
// directly in the output file.

module Module (
  // global clock
  input logic clock,
  // output registers
  int my_reg
);
/*public:*/

  always_ff @(posedge clock) begin : tick
    my_reg <= my_reg + 1;
  end

/*#
  always @(posedge clock) begin
    //$display("Hello World!\n");
  end
#*/

endmodule
