`include "metron_tools.sv"

// Cramming various statements into one line should not break anything.

module Module
(
  input logic clock,
  output logic[7:0] my_reg,
  output logic[7:0] tock_ret
);
/*public:*/

  function logic[7:0] tock();  logic[7:0] a;
a = 1; a = a + 7; tock = a; endfunction

  function void tick();  if (my_reg & 1) my_reg <= my_reg - 7; endfunction

  //----------------------------------------
  always_comb begin
    tock_ret = tock();
  end
  always_ff @(posedge clock) begin
    tick();
  end

endmodule
