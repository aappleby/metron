`include "metron_tools.sv"

// Calling tick() from tock() in the same module should work and should
// generate bindings for the tick() call.

module Module
(
  input logic clock,
  input logic[7:0] tock_val
);
/*public:*/

  function void tock(logic[7:0] val);
    tick_val = val;
    /*tick(val)*/;
  endfunction

/*private:*/

  function void tick(logic[7:0] val);
    my_reg <= my_reg + val;
  endfunction

  logic[7:0] my_reg;

  always_comb begin
    tock(tock_val);
  end

  logic[7:0] tick_val;

  always_ff @(posedge clock) begin
    tick(tick_val);
  end


endmodule
