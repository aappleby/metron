`include "metron_tools.sv"

// Tocks should be able to call private tasks and functions

module Module
(
  input logic clock,
  output logic[7:0] my_signal
);
/*public:*/


  function void tock();
    /*set_signal(get_number())*/;
  endfunction

  function logic[7:0] get_number();
    get_number = 7;
  endfunction

  function void set_signal(logic[7:0] number);
    my_signal = number;
  endfunction

  always_comb begin
    tock();
  end

  always_ff @(posedge clock) begin
  end


endmodule
