`include "metron/metron_tools.sv"

module alu_control (
  // output signals
  output logic[4:0] op_funct
);
 /*public:*/
  logic[6:0] inst_funct7;

 /*public:*/
  always_comb begin : tock_alu_function

    if (inst_funct7[5])
      op_funct = 1;
    else
      op_funct = 2;
  end
endmodule
