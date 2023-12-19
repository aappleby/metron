`include "metron/metron_tools.sv"

// Calling tick() from tock() in the same module should work and should
// generate bindings for the tick() call.

module Module
(
  // global clock
  input logic clock,
  // tock() ports
  input logic[7:0] tock_val
);
/*public:*/

  always_ff @(posedge clock) begin : tock
    tick(tock_val);
  end

/*private:*/

  task automatic tick(logic[7:0] val);
    my_reg_ <= my_reg_ + val;
  endtask

  logic[7:0] my_reg_;

endmodule
