`include "metron_tools.sv"

// Public signal member variables get moved to the output port list.

module Module
(
  output logic my_sig
);
/*public:*/

  task automatic tock();
    my_sig = 1;
  endtask
  always_comb tock();

endmodule
