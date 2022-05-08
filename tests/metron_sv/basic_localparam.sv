`include "metron_tools.sv"

// Static const members become SV localparams

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

  localparam int my_val = 7;

  function void tick();
    my_reg <= my_reg + my_val;
  endfunction
  always_ff @(posedge clock) tick();

  logic[6:0] my_reg;
endmodule
