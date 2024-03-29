`include "metron/metron_tools.sv"

// Private non-const methods should turn into SV tasks.

module Module
(
  // global clock
  input logic clock
);
/*public:*/

  always_ff @(posedge clock) begin : tock
    tick();
  end

/*private:*/

  task automatic tick();
    my_reg_ <= my_reg_ + my_reg2_ + 3;
    some_task2();
  endtask

  task automatic some_task2();
    my_reg2_ <= my_reg2_ + 3;
  endtask

  logic[7:0] my_reg_;
  logic[7:0] my_reg2_;
endmodule
