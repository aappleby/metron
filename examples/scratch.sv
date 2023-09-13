`include "metron/tools/metron_tools.sv"

// Public register member variables get moved to the output port list.

module Module (
  // global clock
  input logic clock,
  // output registers
  logic my_reg
);
/*public:*/

  always_ff @(posedge clock) begin : tick
    if (1) begin
      my_reg <= my_reg + 2;
    end
    else begin
      my_reg <= my_reg + 1;
    end
  end

  logic my_reg;
endmodule