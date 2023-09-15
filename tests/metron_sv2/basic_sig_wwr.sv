`include "metron/tools/metron_tools.sv"

// Writing a register multiple times in the same function is OK.

module Module (
);
/*public:*/

  always_comb begin : tock
    logic temp;
    my_sig = 0;
    my_sig = 1;
    temp = my_sig;
  end

endmodule
