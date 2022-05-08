`include "metron_tools.sv"

// Writing a register multiple times in the same function is OK.

module Module
(
  input logic clock
);
/*public:*/

  function void tock();
    /*tick()*/;
  endfunction

/*private:*/

  function void tick();
    logic temp;
    temp = my_reg;
    my_reg <= 0;
    my_reg <= 1;
  endfunction

  logic my_reg;

  //----------------------------------------

  always_comb tock();
  always_ff @(posedge clock) tick();

endmodule
