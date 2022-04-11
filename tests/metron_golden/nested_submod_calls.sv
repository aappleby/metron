`include "metron_tools.sv"

// Nesting submod calls should put all the nested input bindings above the call.

module Submod1
(
  input logic clock,
  input logic[7:0] add_one_a,
  output logic[7:0] add_one
);
/*public:*/
  always_comb begin
    add_one = add_one_a + 1;
  end
endmodule

module Submod2
(
  input logic clock,
  input logic[7:0] add_two_a,
  output logic[7:0] add_two
);
/*public:*/
  always_comb begin
    add_two = add_two_a + 1;
  end
endmodule

module Module
(
  input logic clock,
  input logic[7:0] tock_old_counter,
  output logic[7:0] tock
);
/*public:*/

  always_comb begin /*tock*/
    logic[7:0] new_counter;

    // Two bindings should end up here.
    submod2_add_two_a = tock_old_counter;
    submod1_add_one_a = submod2_add_two;
    new_counter = submod1_add_one;
    tock = new_counter;
  end

/*private:*/

  Submod1 submod1(
    // Inputs
    .clock(clock),
    .add_one_a(submod1_add_one_a),
    // Outputs
    .add_one(submod1_add_one)
  );
  logic[7:0] submod1_add_one_a;
  logic[7:0] submod1_add_one;

  Submod2 submod2(
    // Inputs
    .clock(clock),
    .add_two_a(submod2_add_two_a),
    // Outputs
    .add_two(submod2_add_two)
  );
  logic[7:0] submod2_add_two_a;
  logic[7:0] submod2_add_two;

endmodule

