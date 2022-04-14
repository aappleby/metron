`include "metron_tools.sv"

// If input ports get deduped, we have to somehow ensure that all bindings to
// the deduped port are the same...?

module Submod
(
  input logic clock
);
/*public:*/

  function logic[7:0] thing1(logic[7:0] thing1_dedup);
    thing1 = thing1_dedup + 1;
  endfunction

  function logic[7:0] thing2(logic[7:0] thing2_dedup);
    thing2 = thing2_dedup + 2;
  endfunction
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
    .clock(clock)
    // Outputs
  );


endmodule

