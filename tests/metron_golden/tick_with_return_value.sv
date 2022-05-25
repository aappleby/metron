`include "metron_tools.sv"

// Tick methods are not allowed to have a return value.
// X Tick method Module.tick is not allowed to have a return value.

module Module (
  // global clock
  input logic clock,
  // tick() ports
  output logic[1:0] tick_ret
);
 /*public:*/
  always_ff @(posedge clock) begin : tick
    my_reg <= my_reg + 1;
    tick_ret <= 8;
  end

 /*private:*/
  logic[1:0] my_reg;
endmodule
