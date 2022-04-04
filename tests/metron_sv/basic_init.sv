`include "metron_tools.sv"

// Modules can use init() methods to initialize state

module Module
(
  input logic clock,
  output logic[7:0] tock
);
/*public:*/

  initial begin /*init*/
    my_reg = 7;
  end

  always_comb begin /*tock*/
    tock = my_reg;
  end

/*private:*/

  logic[7:0] my_reg;
endmodule

