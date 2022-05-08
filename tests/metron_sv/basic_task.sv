`include "metron_tools.sv"

// Private non-const methods should turn into SV tasks.

module Module
(
  input logic clock
);
/*public:*/

  task  tock();
    tick();
  endtask
  always_comb tock();

/*private:*/

  task  tick();
    /*some_task()*/;
  endtask

  task  some_task();
    my_reg <= my_reg + my_reg2 + 3;
    some_task2();
  endtask
  always_ff @(posedge clock) some_task();

  task  some_task2();
    my_reg2 <= my_reg2 + 3;
  endtask

  logic[7:0] my_reg;
  logic[7:0] my_reg2;
endmodule
