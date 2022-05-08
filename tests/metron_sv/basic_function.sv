`include "metron_tools.sv"

// Private const methods should turn into SV functions.

module Module
(
  input logic clock
);
/*public:*/

  task  tock();
    /*tick()*/;
  endtask
  always_comb tock();

/*private:*/

  task  tick();
    my_reg <= my_reg + some_func();
  endtask
  always_ff @(posedge clock) tick();

  function logic[7:0] some_func();
    some_func = 3;
  endfunction

  logic[7:0] my_reg;
endmodule
