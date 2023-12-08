`include "metron/metron_tools.sv"

// Cramming various statements into one line should not break anything.

module Module (
  // global clock
  input logic clock,
  // test() ports
  output logic[7:0] test_ret
);
/*public:*/

  always_comb begin : test
    logic[7:0] a; a = 1; a = a + 7; test_ret = a; end

  always_ff @(posedge clock) begin : tick  if (my_reg_ & 1) my_reg_ <= my_reg_ - 7; end


endmodule
