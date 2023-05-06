`include "metron/tools/metron_tools.sv"

// Public signal member variables get moved to the output port list.

module Module  (
  // output signals
  output logicmy_sig
);
/*public*/

  always_comb begin : tock
    my_sig = 1;
  end

endmodule
