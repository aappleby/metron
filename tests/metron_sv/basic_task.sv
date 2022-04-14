`include "metron_tools.sv"

// Private non-const methods should turn into SV tasks.

module Module
(
  input logic clock
);
/*public:*/

  always_ff @(posedge clock) begin /*tick*/
    some_task();
  end

/*private:*/

  task some_task();
    my_reg = my_reg + 3;
  endtask

  logic[7:0] my_reg;
endmodule

