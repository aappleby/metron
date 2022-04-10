`include "metron_tools.sv"

// Multiple tick methods are OK as long as they're all called by tock()

module Module
(
  input logic clock,
  output logic[7:0] tock
);

  always_comb begin /*tock*/
    logic[7:0] result;

    result = my_reg1 + my_reg2;
    /*tick1()*/;
    /*tick2()*/;
    tock = result;
  end


  task tick1();
    my_reg1 <= 0;
  endtask
  always_ff @(posedge clock) tick1();

  task tick2();
    my_reg2 <= 1;
  endtask
  always_ff @(posedge clock) tick2();

  logic[7:0] my_reg1;
  logic[7:0] my_reg2;
endmodule

