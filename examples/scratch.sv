`include "metron/metron_tools.sv"

// Tocks should be able to call private tasks and functions

module Module (
  // output signals
  output logic[7:0] my_signal
);
/*public:*/


  always_comb begin : tock
    set_signal(get_number());
  end

/*private:*/

  function logic[7:0] get_number() /*const*/;
    get_number = 7;
  endfunction

  task automatic set_signal(logic[7:0] number);
    my_signal = number;
  endtask

endmodule
