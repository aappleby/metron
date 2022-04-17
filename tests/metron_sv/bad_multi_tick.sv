`include "metron_tools.sv"

// Multiple tick methods that write the same reg are not allowed.

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
    reg1 <= 1;
  end

  logic reg1;
endmodule

