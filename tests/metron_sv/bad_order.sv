`include "metron_tools.sv"

// Declaration order _matters_ - a tick() before a tock() that reads the ticked
// reg should fail validation.

module Module
(
  input logic clock,
  output logic sig,
  input logic reg
);
/*public:*/

  always_ff @(posedge clock) begin /*tick*/
    reg = 1;
  end

  always_comb begin /*tock*/
    sig = reg;
  end

endmodule

