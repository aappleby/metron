`include "metron_tools.sv"

// Submodule bindings should be placed immediately before the statement
// containing the call

module Submod
(
  input logic clock,
  input logic[7:0] sum_a_a1,
  input logic[7:0] sum_a_a2,
  input logic[7:0] sum_b_b1,
  input logic[7:0] sum_b_b2,
  output logic[7:0] sum_a,
  output logic[7:0] sum_b
);
/*public:*/

  always_comb begin sum_a = sum_a_a1 + sum_a_a2; end
  always_comb begin sum_b = sum_b_b1 + sum_b_b2; end
endmodule

module Module
(
  input logic clock,
  output logic[7:0] test_bindings
);
/*public:*/

  always_comb begin
    logic[7:0] result;

    // Submod bindings should _not_ end up here.

    begin
      // Only sum_a's bindings should be here.
      submod_sum_a_a1 = 1;
      submod_sum_a_a2 = 2;
      if (submod_sum_a) begin
        // Only sum_b's bindings should be here.
        submod_sum_b_b1 = 3;
        submod_sum_b_b2 = 4;
        result = submod_sum_b;
      end
      else begin
        // Only sum_b's bindings should be here.
        submod_sum_b_b1 = 5;
        submod_sum_b_b2 = 6;
        result = submod_sum_b;
      end
    end

    test_bindings = result;
  end

  Submod submod(
    // Inputs
    .clock(clock),
    .sum_a_a1(submod_sum_a_a1),
    .sum_a_a2(submod_sum_a_a2),
    .sum_b_b1(submod_sum_b_b1),
    .sum_b_b2(submod_sum_b_b2),
    // Outputs
    .sum_a(submod_sum_a),
    .sum_b(submod_sum_b)
  );
  logic[7:0] submod_sum_a_a1;
  logic[7:0] submod_sum_a_a2;
  logic[7:0] submod_sum_b_b1;
  logic[7:0] submod_sum_b_b2;
  logic[7:0] submod_sum_a;
  logic[7:0] submod_sum_b;

endmodule

