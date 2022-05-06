`include "metron_tools.sv"

// Template parameters become SV module parameters.

module Module
#(parameter int SOME_CONSTANT = 7)
(
  input logic clock
);
/*public:*/

  always_comb begin /*tock*/
    /*tick()*/;
  end

/*private:*/

  always_comb begin /*tick*/
    my_reg = SOME_CONSTANT;
  end

  logic[6:0] my_reg;
endmodule
