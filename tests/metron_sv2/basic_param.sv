`include "metron/tools/metron_tools.sv"

// Template parameters become SV module parameters.

module Module (
  // global clock
  input logic clock
);
  parameter SOME_CONSTANT = 7;

/*public:*/

  always_comb begin : tock
    /*tick();*/
  end

/*private:*/

  always_ff @(posedge clock) begin : tick
    my_reg <= my_reg + SOME_CONSTANT;
  end

  logic[6:0] my_reg;
endmodule
