`include "metron/metron_tools.sv"

// Writing a register multiple times in the same function is OK.

module Module (
  // output signals
  output logic my_sig
);
/*public:*/

  always_comb begin : tock
    logic temp;
    my_sig = 0;
    my_sig = 1;
    temp = my_sig;
  end

endmodule
