`include "metron_tools.sv"

// Template parameters become SV module parameters.

module Module
#(
parameter int SOME_CONSTANT = 7
)
(
  // global clock
  input logic clock
);
/*public:*/

  always_comb begin : tock
  end

/*private:*/

  always_ff @(posedge clock) begin : tick
    my_reg <= my_reg + SOME_CONSTANT;
  end

  logic[6:0] my_reg;
endmodule
