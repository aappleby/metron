`include "metron_tools.sv"

// Getter methods should turn into outputs.

module Module
(
  input logic clock,
  output logic[6:0] tock_get_reg
);
/*public:*/

  always_comb begin /*tock_get_reg*/
    tock_get_reg = my_reg;
  end

  always_comb begin /*tock*/
    /*tick()*/;
  end


/*private:*/

  always_ff @(posedge clock) begin /*tick*/
    my_reg <= my_reg + 1;
  end

  logic[6:0] my_reg;
endmodule

