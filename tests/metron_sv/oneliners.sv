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
  always_comb tock_ret = tock();

  task  tick();  if (my_reg & 1) my_reg <= my_reg - 7; endtask
  always_ff @(posedge clock) tick();


endmodule
