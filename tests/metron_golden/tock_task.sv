`include "metron_tools.sv"

// Tocks should be able to call private tasks and functions

module Module
(
  output logic[7:0] my_signal,
  output int tock_ret
);
/*public:*/


  always_comb begin : tock
    tock_ret = set_signal(get_number());
  end

  function logic[7:0] get_number();
    get_number = 7;
  endfunction

  function int set_signal(logic[7:0] number);
    my_signal = number;
    set_signal = my_signal;
  endfunction

endmodule
