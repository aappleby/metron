`include "metron_tools.sv"

// Registers are "locked" when the function that writes to them exits and
// cannot be written in another function.

module Module
(
  input logic clock
);
/*public:*/

  always_comb begin /*tock*/
    /*tick1()*/;
    /*tick2()*/;
  end

/*private:*/

  always_ff @(posedge clock) begin /*tick1*/
    reg1 <= 0;
  end

  always_ff @(posedge clock) begin /*tick2*/
    reg1 <= 0;
    reg2 <= 1;
  end

  logic reg1;
  logic reg2;
endmodule

