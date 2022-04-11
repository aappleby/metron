`include "metron_tools.sv"

// If input ports get deduped, we have to somehow ensure that all bindings to
// the deduped port are the same...?

module Submod
(
  input logic clock,
  input logic[7:0] thing1_dedup,
  input logic[7:0] thing2_dedup,
  output logic[7:0] thing1,
  output logic[7:0] thing2
);
/*public:*/

  always_comb begin
    thing1 = thing1_dedup + 1;
  end

  always_comb begin
    thing2 = thing2_dedup + 2;
  end
endmodule

module Module
(
  input logic clock,
  output logic[7:0] tock
);
/*public:*/

  always_comb begin /*tock*/
    logic[7:0] dedup1;
    logic[7:0] dedup2;

    dedup1 = 10;
    dedup2 = 20;

    submod_thing1_dedup = dedup1;
    submod_thing2_dedup = dedup2;
    tock = submod_thing1 + submod_thing2;
  end

  Submod submod(
    // Inputs
    .clock(clock),
    .thing1_dedup(submod_thing1_dedup),
    .thing2_dedup(submod_thing2_dedup),
    // Outputs
    .thing1(submod_thing1),
    .thing2(submod_thing2)
  );
  logic[7:0] submod_thing1_dedup;
  logic[7:0] submod_thing2_dedup;
  logic[7:0] submod_thing1;
  logic[7:0] submod_thing2;


endmodule

