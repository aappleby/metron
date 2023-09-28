`include "metron/tools/metron_tools.sv"

// A method that writes both a signal and a register should fail

module Module (
  // update() ports
  input logic update_in,
  input int update_lksjf
);
 /*public:*/

  always_comb begin : update
    sig = reg + update_in;
    reg = sig + update_in;
  end

endmodule
