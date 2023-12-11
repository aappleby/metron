`include "metron/metron_tools.sv"

// Writing a register multiple times in the same function is OK.

module Module (
  // global clock
  input logic clock,
  // output registers
  output logic my_reg_
);
/*public:*/

  always_ff @(posedge clock) begin : tick
    logic temp;
    temp = my_reg_;
    my_reg_ <= 0;
    my_reg_ <= 1;
  end

endmodule
