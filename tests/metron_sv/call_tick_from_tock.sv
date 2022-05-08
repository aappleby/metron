`include "metron_tools.sv"

// Calling tick() from tock() in the same module should work and should
// generate bindings for the tick() call.

module Module
(
  input logic clock,
  input logic[7:0] tock_val
);
/*public:*/

  task  tock(logic[7:0] val);
    tick_val = val;
    /*tick(val)*/;
  endtask
  always_comb tock(tock_val);

/*private:*/

  task  tick(logic[7:0] val);
    my_reg <= my_reg + val;
  endtask
  logic[7:0] tick_val;
  always_ff @(posedge clock) tick(tick_val);

  logic[7:0] my_reg;

endmodule
