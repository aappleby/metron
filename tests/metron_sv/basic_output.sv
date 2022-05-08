`include "metron_tools.sv"

// Getter methods should turn into outputs.

module Module
(
  input logic clock,
  output logic[6:0] tock_get_reg_ret
);
/*public:*/

  function logic[6:0] tock_get_reg();
    tock_get_reg = my_reg;
  endfunction
  always_comb tock_get_reg_ret = tock_get_reg();

  function void tock();
    /*tick()*/;
  endfunction
  always_comb tock();


/*private:*/

  function void tick();
    my_reg <= my_reg + 1;
  endfunction
  always_ff @(posedge clock) tick();

  logic[6:0] my_reg;
endmodule
