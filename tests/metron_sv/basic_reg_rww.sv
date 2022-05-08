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
  always_comb tock();

/*private:*/

  function void tick();
    logic temp;
    temp = my_reg;
    my_reg <= 0;
    my_reg <= 1;
  endfunction
  always_ff @(posedge clock) tick();

  logic my_reg;
endmodule
