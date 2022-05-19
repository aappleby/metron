`include "metron_tools.sv"

// Private non-const methods should turn into SV tasks.

module Module
(
  input logic clock,
  output int tock_ret
);
/*public:*/

  always_comb begin : tock
    tock_ret = 0;
  end

/*private:*/

  task automatic tick();
    my_reg <= my_reg + my_reg2 + 3;
    some_task2();
  endtask
  always_ff @(posedge clock) tick();

  task automatic some_task2();
    my_reg2 <= my_reg2 + 3;
  endtask

  logic[7:0] my_reg;
  logic[7:0] my_reg2;
endmodule
