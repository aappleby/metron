`include "metron_tools.sv"

// Comments surrounded by / * # <something # * / get unwrapped and dropped
// directly in the output file.

module Module
(
  input logic clock
);
/*public:*/

  function void tock();
  endfuction


  always @(posedge clock) begin
    $display("Hello World!\n");
  end


endmodule;

