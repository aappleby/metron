`include "metron_tools.sv"

// Template parameters become SV module parameters.

module Module
#(parameter int SOME_CONSTANT = 7)
(
  input logic clock
);
/*public:*/

  function tock();
  endfunction
  always_comb tock();

/*private:*/

  task automatic tick();
    my_reg <= my_reg + SOME_CONSTANT;
  endtask
  always_ff @(posedge clock) tick();

  logic[6:0] my_reg;
endmodule
