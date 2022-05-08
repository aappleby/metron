`include "metron_tools.sv"

// Writing a register multiple times in the same function is OK.

module Module
(
  input logic clock
);
/*public:*/

  task automatic tock();
    /*tick()*/;
  endtask
  always_comb tock();

/*private:*/

  task automatic tick();
    logic temp;
    temp = my_reg;
    my_reg <= 0;
    my_reg <= 1;
  endtask
  always_ff @(posedge clock) tick();

  logic my_reg;
endmodule
