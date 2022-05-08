`include "metron_tools.sv"

// Declaration order _matters_ - a tock() that reads a reg before the tick()
// that writes it is OK.

module Module
(
  input logic clock
);
/*public:*/

  function void tock();
    my_sig = my_reg;
    /*tick()*/;
  endfunction
  always_comb tock();

/*private:*/

  function void tick();
    my_reg <= 1;
  endfunction
  always_ff @(posedge clock) tick();

  logic my_sig;
  logic my_reg;
endmodule
