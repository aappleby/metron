`include "metron_tools.sv"

// Increment/decrement should be translated into equivalent Verilog, but they
// do _not_ return the old/new value.

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
    my_reg1 = my_reg1 + 1;
    my_reg2 = my_reg2 + 1;
    my_reg3 = my_reg3 - 1;
    my_reg4 = my_reg4 - 1;
  end

  int my_reg1;
  int my_reg2;
  int my_reg3;
  int my_reg4;
endmodule;

