`include"metron_tools.sv"

// Getter methods should turn into outputs.

module Module
(
  input logic clock,
  output logic[6:0]tock_get_reg_ret
);
/*public:*/

  function logic[6:0] tock_get_reg();
    tock_get_reg = my_reg;
  endfuction
  always_comb tock_get_reg_ret = tock_get_reg();

  always_ff @(posedge clock) begin /*tock*/
    /*tick()*/;
  end


/*private:*/

  always_ff @(posedge clock) begin /*tick*/
    my_reg = my_reg + 1;
  end

  logic[6:0] my_reg;
endmodule;

