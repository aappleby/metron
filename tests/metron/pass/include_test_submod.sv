// Metron should be able to handle #included submodules.

`include "metron/metron_tools.sv"

module Submod
(
  // global clock
  input logic clock,
  // tick() ports
  input int tick_x
);
/*public:*/

  always_ff @(posedge clock) begin : tick
    y_ <= y_ + tick_x;
  end

/*private:*/
  int y_;
endmodule
