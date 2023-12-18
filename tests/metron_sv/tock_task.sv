`include "metron/metron_tools.sv"

// Tocks should be able to call private tasks and functions

module Module
(
  // output signals
  output logic[7:0] my_signal,
  output int tock_ret
);
/*public:*/


  always_comb begin : tock
    set_signal(get_number());
    tock_ret = set_signal_ret;
  end

/*private:*/

  function logic[7:0] get_number() /*const*/;
    get_number = 7;
  endfunction

  int set_signal_ret;
  task automatic set_signal(logic[7:0] number);
    my_signal = number;
    set_signal_ret = my_signal;
  endtask

endmodule
