`include "metron/tools/metron_tools.sv"

// Tocks should be able to call private tasks and functions

module Module  (
  // output signals
  output logic[7:0]my_signal,
  // tock() ports
  output int tock_ret
);
/*public*/


  always_comb begin : tock
    set_signal_number = get_number();tock_ret = set_signal_ret;
  end

  function logic[7:0] get_number();
    get_number = 7;
  endfunction

  always_comb begin : set_signal
    my_signal = set_signal_number;
    set_signal_ret = my_signal;
  end
  logic[7:0] set_signal_number;
  int set_signal_ret;

endmodule
