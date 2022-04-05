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
  input logic[7:0] d1,
  input logic[7:0] d2,
  input logic[7:0] e1,
  input logic[7:0] e2,
  output logic[7:0] sum_a,
  output logic[7:0] sum_b,
  output logic[7:0] sum_d,
  output logic[7:0] sum_e
);
/*public:*/

  always_comb begin sum_a = a1 + a2; end
  always_comb begin sum_b = b1 + b2; end
  always_comb begin sum_d = d1 + d2; end
  always_comb begin sum_e = e1 + e2; end
endmodule

module Module
(
  input logic clock,
  output logic[7:0] test_bindings
);
/*public:*/

  always_comb begin
    logic[7:0] result;
    /*logic<8> result;*/
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
        // Only sum_c's bindings should be here.
        submod_b1 = 5;
        submod_b2 = 6;
        result = submod_sum_b;
      end

      // All of d/e/f's bindings should be here.
      submod_d1 = 7;
      submod_d2 = 8;
      submod_e1 = 9;
      submod_e2 = 10;
      submod_e1 = 11;
      submod_e2 = 12;
      if (submod_sum_d)
        result = submod_sum_e;
      else
        result = submod_sum_e;
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
    .d1(submod_d1), 
    .d2(submod_d2), 
    .e1(submod_e1), 
    .e2(submod_e2), 
    // Outputs
    .sum_a(submod_sum_a), 
    .sum_b(submod_sum_b), 
    .sum_d(submod_sum_d), 
    .sum_e(submod_sum_e)
  );
  logic[7:0] submod_a1;
  logic[7:0] submod_a2;
  logic[7:0] submod_b1;
  logic[7:0] submod_b2;
  logic[7:0] submod_d1;
  logic[7:0] submod_d2;
  logic[7:0] submod_e1;
  logic[7:0] submod_e2;
  logic[7:0] submod_sum_a;
  logic[7:0] submod_sum_b;
  logic[7:0] submod_sum_d;
  logic[7:0] submod_sum_e;

endmodule

