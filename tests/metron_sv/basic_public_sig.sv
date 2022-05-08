`include "metron_tools.sv"

// Public signal member variables get moved to the output port list.

module Module
(
  output logic my_sig
);
/*public:*/

  function void tock();
    my_sig = 1;
  endfunction
  always_comb tock();

endmodule
