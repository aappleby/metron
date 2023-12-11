`include "metron/metron_tools.sv"

// Methods that don't write anything should become functions.

module Module (
  // global clock
  input logic clock,
  // output registers
  output logic[7:0] my_reg_
);
/*public:*/

  always_ff @(posedge clock) begin : tick
    my_reg_ <= my_reg_ + some_func();
  end

  function logic[7:0] some_func() /*const*/;
    some_func = 3;
  endfunction

endmodule
