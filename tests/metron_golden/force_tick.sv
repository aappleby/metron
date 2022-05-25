// Prefixing a method with "tick_" should force it to be a tick if it would
// otherwise be ambiguous

module Adder (
  // global clock
  input logic clock,
  // tick_add() ports
  input int tick_add_a,
  input int tick_add_b,
  output int tick_add_ret
);
/*public:*/

  always_ff @(posedge clock) begin : tick_add
    tick_add_ret <= tick_add_a + tick_add_b;
  end
endmodule
