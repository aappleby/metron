`include "metron_tools.sv"

// Writing a register multiple times in the same function is OK.

module Module
(
  input logic clock,
  output logic my_reg
);
/*public:*/

  always_ff @(posedge clock) begin : tick
    logic temp;

    temp = my_reg;
    my_reg <= 0;
    my_reg <= 1;
  end

endmodule

