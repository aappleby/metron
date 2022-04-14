`include "metron_tools.sv"

// Getter methods should turn into outputs.

module Module
(
  input logic clock,
  output logic[6:0] my_reg
);
/*public:*/

  function logic[6:0] get_reg();
    get_reg = my_reg;
  endfunction

  always_ff @(posedge clock) begin /*tick*/
    my_reg <= my_reg + 1;
  end

endmodule

