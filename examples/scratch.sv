// If "sum" does not have an underscore suffix, it is a signal and "add" will
// be converted to an always_comb block.
module Adder1 (
  // output signals
  output int sum,
  // add() ports
  input int add_a,
  input int add_b
);
/*public:*/
  always_comb begin : add
    sum = add_a + add_b;
  end
endmodule

// If "sum" does have an underscore suffix, it is a register and "add" will
// be converted to an always_ff block.
module Adder2 (
  // global clock
  input logic clock,
  // output registers
  output int sum_,
  // add() ports
  input int add_a,
  input int add_b
);
/*public:*/
  always_ff @(posedge clock) begin : add
    sum_ <= add_a + add_b;
  end
endmodule
