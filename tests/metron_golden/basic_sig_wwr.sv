`include "metron_tools.sv"

// Writing a register multiple times in the same function is OK.

module Module
(
  input logic clock,
  output logic my_sig
);
/*public:*/

  function void tock();
    logic temp;
    my_sig = 0;
    my_sig = 1;
    temp = my_sig;
  endfunction


  //----------------------------------------
  always_comb begin
    tock();
  end

endmodule
