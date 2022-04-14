`include "metron_tools.sv"

module Module
(
  input logic clock,
  output int foo
);
/*public:*/

  always_comb begin
    foo = 2;
  end

  always_comb begin /*tock*/
    /*tick()*/;
  end

/*private:*/

  logic[2:0] my_reg;

  always_ff @(posedge clock) begin : tick
    my_reg <= foo() + 1;
  end

endmodule

