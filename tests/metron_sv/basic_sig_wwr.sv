`include "metron_tools.sv"

// Writing a register multiple times in the same function is OK.

module Module
(
  output logic my_sig
);
/*public:*/

  task  tock();
    logic temp;
    my_sig = 0;
    my_sig = 1;
    temp = my_sig;
  endtask
  always_comb tock();

endmodule
