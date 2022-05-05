`include "metron_tools.sv"

// Private non-const methods should turn into SV tasks.

module Module
(
  input logic clock
);
/*public:*/

  always_comb begin /*tock*/
    /*tick()*/;
  end

/*private:*/

  always_comb begin /*tick*/
    /*some_task()*/;
  end

  always_ff @(posedge clock) begin /*some_task*/
    my_reg <= my_reg + my_reg2 + 3;
    /*some_task2()*/;
  end

  always_ff @(posedge clock) begin /*some_task2*/
    my_reg2 <= my_reg2 + 3;
  end

  logic[7:0] my_reg;
  logic[7:0] my_reg2;
endmodule

