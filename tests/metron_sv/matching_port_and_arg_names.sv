`include "metron_tools.sv"

// Port and function arg names can collide, the latter is disambiguated by its
// function name.

module Module
(
  input logic clock,
  input logic[2:0] input_val,
  output logic[2:0] output_val,
  input logic[2:0] tock_input_val,
  output logic[2:0] tock_ret
);
/*public:*/


  function void tock1();
    output_val = input_val + 7;
  endfunction

  function logic[2:0] tock(logic[2:0] input_val);
    tock = input_val + 8;
  endfunction
  //----------------------------------------
  always_comb begin
    tock1();
    tock_ret = tock(tock_input_val);
  end
endmodule
