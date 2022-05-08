`include "metron_tools.sv"

// Private const methods should turn into SV functions.

module Module
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
    my_reg <= my_reg + some_func();
  endfunction
  always_ff @(posedge clock) tick();

  function logic[7:0] some_func();
    some_func = 3;
  endfunction

  logic[7:0] my_reg;
endmodule
