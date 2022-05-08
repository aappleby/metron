`include "metron_tools.sv"

// Public signal member variables get moved to the output port list.

module Module
(
  input logic clock,
  output logic my_sig
);
/*public:*/

  function void tock();
    my_sig = 1;
  endfunction


  //----------------------------------------
  always_comb begin
    tock();
  end

endmodule
