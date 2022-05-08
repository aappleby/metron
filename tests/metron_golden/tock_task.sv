`include "metron_tools.sv"

// Tocks should be able to call private tasks and functions

module Module
(
  output logic[7:0] my_signal
);
/*public:*/


  function tock();
    set_signal(get_number());
  endfunction
  always_comb tock();

  function logic[7:0] get_number();
    get_number = 7;
  endfunction

  function set_signal(logic[7:0] number);
    my_signal = number;
  endfunction

endmodule
