`include "metron_tools.sv"

// Comments surrounded by / * # <something # * / get unwrapped and dropped
// directly in the output file.

module Module
(
  input logic clock,
  output int my_reg
);
/*public:*/

  task automatic tick();
    my_reg <= my_reg + 1;
  endtask
  always_ff @(posedge clock) tick();


  always @(posedge clock) begin
    $display("Hello World!\n");
  end


endmodule
