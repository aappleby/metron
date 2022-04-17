`include "metron_tools.sv"

// All submod input ports have to be bound.
// FIXME - Maybe only tick/tock ports have to be bound?

module Submod
(
  input logic clock
);
/*public:*/

  function logic foo(logic foo_a);
    foo = !foo_a;
  endfunction

  function logic bar(logic bar_b);
    bar = !bar_b;
  endfunction
endmodule

module Module
(
  input logic clock
);
/*public:*/

  always_ff @(posedge clock) begin /*tick*/
    submod_foo_a = 1;
    /*submod.foo(1);*/
  end

/*private:*/
  Submod submod(
    // Inputs
    .clock(clock)
    // Outputs
  );

endmodule

