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
    logic result;
    import rv_constants::*;
    /*logic<1> result;*/
    case (inst_funct3) 
      /*case*/ FUNCT3_BRANCH_EQ:  result = !result_equal_zero; /*break;*/
      /*case*/ FUNCT3_BRANCH_NE:  result = result_equal_zero; /*break;*/
      /*case*/ FUNCT3_BRANCH_LT:  result = !result_equal_zero; /*break;*/
      /*case*/ FUNCT3_BRANCH_GE:  result = result_equal_zero; /*break;*/
      /*case*/ FUNCT3_BRANCH_LTU: result = !result_equal_zero; /*break;*/
      /*case*/ FUNCT3_BRANCH_GEU: result = result_equal_zero; /*break;*/
      default:                result = 1'bx; /*break;*/
    endcase
    take_branch = result;
  end
endmodule;

`endif  // RVSIMPLE_CONTROL_TRANSFER_H

