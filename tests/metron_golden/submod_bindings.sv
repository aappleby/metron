`include "metron_tools.sv"

// Submodule bindings should be placed immediately before the statement
// containing the call

module Submod
(
  input logic clock,
  input logic[7:0] a1,
  input logic[7:0] a2,
  input logic[7:0] b1,
  input logic[7:0] b2,
  output logic[7:0] sum_a,
  output logic[7:0] sum_b
);

  always_comb begin sum_a = a1 + a2; end
  always_comb begin sum_b = b1 + b2; end
endmodule

module Module
(
  input logic clock,
  output logic[7:0] test_bindings
);

  always_comb begin
    logic[7:0] result;

    // Submod bindings should _not_ end up here.

    begin
      // Only sum_a's bindings should be here.
      submod_a1 = 1;
      submod_a2 = 2;
      if (submod_sum_a) begin
        // Only sum_b's bindings should be here.
        submod_b1 = 3;
        submod_b2 = 4;
        result = submod_sum_b;
      end
      else begin
        // Only sum_b's bindings should be here.
        submod_b1 = 5;
        submod_b2 = 6;
        result = submod_sum_b;
      end
    end

    test_bindings = result;
  end

  Submod submod(
    // Inputs
    .clock(clock),
    .a1(submod_a1),
    .a2(submod_a2),
    .b1(submod_b1),
    .b2(submod_b2),
    // Outputs
    .sum_a(submod_sum_a),
    .sum_b(submod_sum_b)
  );
  logic[7:0] submod_a1;
  logic[7:0] submod_a2;
  logic[7:0] submod_b1;
  logic[7:0] submod_b2;
  logic[7:0] submod_sum_a;
  logic[7:0] submod_sum_b;

endmodule

