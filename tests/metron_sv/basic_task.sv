`include "metron_tools.sv"

// Private non-const methods should turn into SV tasks.

module Module
(
  input logic clock
);
/*public:*/

  always_ff @(posedge clock) begin /*tock*/
    /*tick()*/;
  end

/*private:*/

  always_ff @(posedge clock) begin /*tick*/
    /*some_task()*/;
  end

  always_ff @(posedge clock) begin /*some_task*/
    my_reg = my_reg + 3;
  end

  logic[7:0] my_reg;
endmodule

