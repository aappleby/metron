`include "metron_tools.sv"

// Modules can contain other modules.

module Submod
(
  input logic clock
);
/*public:*/

  always_ff @(posedge clock) begin /*tock*/
    /*tick()*/;
  end

/*private:*/

  always_ff @(posedge clock) begin /*tick*/
    sub_reg <= sub_reg + 1;
  end

  logic[7:0] sub_reg;
endmodule

module Module
(
  input logic clock
);
/*public:*/

  always_ff @(posedge clock) begin /*tock*/
    /*submod.tock();*/
  end

  Submod submod(
    // Inputs
    .clock(clock)
    // Outputs
  );

endmodule

