`include "metron_tools.sv"

// Template parameters become SV module parameters.

module Module
#(parameter int SOME_CONSTANT = 7)
(
  input logic clock
);
/*public:*/

  function void tock();
    /*tick()*/;
  endfunction
  always_comb tock();

/*private:*/

  function void tick();
    my_reg <= my_reg + SOME_CONSTANT;
  endfunction
  always_ff @(posedge clock) tick();

  logic[6:0] my_reg;
endmodule
