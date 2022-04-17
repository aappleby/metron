`include "metron_tools.sv"

// Tocks should be able to call private tasks and functions

module Module
(
  input logic clock,
  output logic[7:0] my_signal
);
/*public:*/


  always_comb begin /*tock*/
    set_signal_number = get_number();
    /*set_signal(get_number())*/;
  end

/*private:*/

  function logic[7:0] get_number();
    get_number = 7;
  endfunction

  logic[7:0] set_signal_number;
  always_ff @(posedge clock) begin /*set_signal*/
    my_signal <= set_signal_number;
  end

endmodule

