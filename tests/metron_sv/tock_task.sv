`include"metron_tools.sv"

// Tocks should be able to call private tasks and functions

module Module
(
  input logic clock,
  output logic[7:0] my_signal,
  input logic[7:0] set_signal_number,
  output logic[7:0]get_number_ret
);
/*public:*/


  always_comb begin /*tock*/
    /*set_signal(get_number())*/;
  end

  function logic[7:0] get_number();
    get_number = 7;
  endfuction
  always_comb get_number_ret = get_number();

  always_comb begin /*set_signal*/
    my_signal = number;
  end

endmodule;

