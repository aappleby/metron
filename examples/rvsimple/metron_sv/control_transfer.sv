// RISC-V SiMPLE SV -- control transfer unit
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

`ifndef RVSIMPLE_CONTROL_TRANSFER_H
`define RVSIMPLE_CONTROL_TRANSFER_H

`include "config.sv"
`include "constants.sv"
`include "metron_tools.sv"

module control_transfer
(
  input logic clock,
  input logic result_equal_zero,
  input logic[2:0] inst_funct3,
  output logic take_branch
);
 /*public:*/

  always_comb begin
    import rv_constants::*;
    case (inst_funct3) 
      /*case*/ FUNCT3_BRANCH_EQ:  take_branch = !result_equal_zero;
      /*case*/ FUNCT3_BRANCH_NE:  take_branch = result_equal_zero;
      /*case*/ FUNCT3_BRANCH_LT:  take_branch = !result_equal_zero;
      /*case*/ FUNCT3_BRANCH_GE:  take_branch = result_equal_zero;
      /*case*/ FUNCT3_BRANCH_LTU: take_branch = !result_equal_zero;
      /*case*/ FUNCT3_BRANCH_GEU: take_branch = result_equal_zero;
      default:                take_branch = 1'bx;
    endcase
  end
endmodule;

`endif  // RVSIMPLE_CONTROL_TRANSFER_H
