// Metron should be able to handle #included submodules.

`include "include_test_submod.sv"

module Module
(
  // global clock
  input logic clock,
  // tick() ports
  input int tick_x
);
/*public:*/

  always_comb begin : tick
    submod_tick_x = tick_x;
    /*submod.tick(x);*/
  end

/*private:*/
  Submod submod(
    // Global clock
    .clock(clock),
    // tick() ports
    .tick_x(submod_tick_x)
  );
  int submod_tick_x;
endmodule
