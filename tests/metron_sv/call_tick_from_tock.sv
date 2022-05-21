`include "metron_tools.sv"

// Calling tick() from tock() in the same module should work and should
// generate bindings for the tick() call.

module Module
(
  input logic clock,
  input logic[7:0] tock_val
);
/*public:*/

  always_comb begin : tock
  end

/*private:*/

  always_ff @(posedge clock) begin : tick
    my_reg <= my_reg + tick_val;
  end
  logic[7:0] tick_val;

  logic[7:0] my_reg;

endmodule
