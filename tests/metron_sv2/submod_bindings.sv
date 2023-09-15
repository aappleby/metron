`include "metron/tools/metron_tools.sv"

// Submodule bindings should be placed immediately before the statement
// containing the call

module Submod (
  // sum_a() ports
  input logic[7:0] sum_a_a1,
  input logic[7:0] sum_a_a2,
  output logic[7:0] sum_a_ret,
  // sum_b() ports
  input logic[7:0] sum_b_b1,
  input logic[7:0] sum_b_b2,
  output logic[7:0] sum_b_ret
);
/*public:*/

  always_comb begin : sum_a /*const*/  sum_a_ret = sum_a_a1 + sum_a_a2; end
  always_comb begin : sum_b /*const*/  sum_b_ret = sum_b_b1 + sum_b_b2; end
endmodule

module Module (
  // tock_bindings() ports
  output logic[7:0] tock_bindings_ret
);
/*public:*/

  always_comb begin : tock_bindings /*const*/
    logic[7:0] result;
    result;
    // Submod bindings should _not_ end up here.

    begin
      // Only sum_a's bindings should be here.
      submod_sum_a_a1 = 1;
      submod_sum_a_a2 = 2;
      if (submod_sum_a_ret) begin
        // Only sum_b's bindings should be here.
        submod_sum_b_b1 = 3;
        submod_sum_b_b2 = 4;
        result = submod_sum_b_ret;
      end
      else begin
        // Only sum_b's bindings should be here.
        submod_sum_b_b1 = 5;
        submod_sum_b_b2 = 6;
        result = submod_sum_b_ret;
      end
    end

    tock_bindings_ret = result;
  end

  Submod submod;
endmodule
