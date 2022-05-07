`include "metron_tools.sv"

// I don't know why you would want to do this, but it should work.

module Module
(
  input logic clock
);
/*public:*/
  initial begin
    init1();
  end

  always_comb begin /*tock*/
    /*tick()*/;
  end

/*private:*/

  always_ff @(posedge clock) begin /*tick*/
    reg1 <= reg1 + 1;
    reg2 <= reg2 + 1;
    reg3 <= reg3 + 1;
    reg4 <= reg4 + 1;
    reg5 <= reg5 + 1;
  end

  logic[7:0] reg1;
  logic[7:0] reg2;
  logic[7:0] reg3;
  logic[7:0] reg4;
  logic[7:0] reg5;

  initial begin /*init1*/
    reg1 = 1;
    /*init2()*/;
  end

  initial begin /*init2*/
    reg2 = 2;
    /*init3()*/;
  end

  initial begin /*init3*/
    reg3 = 3;
    /*init4()*/;
  end

  initial begin /*init4*/
    reg4 = 4;
    /*init5()*/;
  end

  initial begin /*init5*/
    reg5 = 5;
  end

endmodule

