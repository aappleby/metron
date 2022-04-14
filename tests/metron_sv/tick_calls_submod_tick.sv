`include "metron_tools.sv"

module Submod
(
  input logic clock
);
/*public:*/

  always_ff @(posedge clock) begin /*tick*/
    my_reg <= my_reg + 1;
  end

/*private:*/
  logic[7:0] my_reg;
endmodule

module Module
(
  input logic clock
);
/*public:*/

  always_ff @(posedge clock) begin /*tick*/
    /*submod.tick();*/
  end

/*private:*/
  Submod submod(
    // Inputs
    .clock(clock)
    // Outputs
  );

endmodule

