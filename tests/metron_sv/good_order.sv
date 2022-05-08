`include "metron_tools.sv"

// Declaration order _matters_ - a tock() that reads a reg before the tick()
// that writes it is OK.

module Module
(
  input logic clock
);
/*public:*/

  task automatic tock();
    my_sig = my_reg;
    /*tick()*/;
  endtask
  always_comb tock();

/*private:*/

  task automatic tick();
    my_reg <= 1;
  endtask
  always_ff @(posedge clock) tick();

  logic my_sig;
  logic my_reg;
endmodule
