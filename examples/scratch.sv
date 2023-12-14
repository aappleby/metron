module Adder (
  // add() ports
  input int add_a,
  input int add_b,
  output int add_ret
);
/*public:*/
  always_comb begin : add
    add_ret = add_a + add_b;
  end
endmodule
