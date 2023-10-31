// RISC-V SiMPLE SV -- control transfer unit
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

`ifndef CONTROL_TRANSFER_H
`define CONTROL_TRANSFER_H

`include "config.sv"
`include "constants.sv"
`include "metron/metron_tools.sv"

module control_transfer (
  // input signals
  input logic result_equal_zero,
  input logic[2:0] inst_funct3,
  // output signals
  output logic take_branch
);
 /*public:*/

  always_comb begin : tock_take_branch
    import rv_constants::*;
    // clang-format off

    // clang-format off
    case (inst_funct3)
      FUNCT3_BRANCH_EQ:   begin take_branch = !result_equal_zero; /*break;*/ end
      FUNCT3_BRANCH_NE:   begin take_branch = result_equal_zero; /*break;*/ end
      FUNCT3_BRANCH_LT:   begin take_branch = !result_equal_zero; /*break;*/ end
      FUNCT3_BRANCH_GE:   begin take_branch = result_equal_zero; /*break;*/ end
      FUNCT3_BRANCH_LTU:  begin take_branch = !result_equal_zero; /*break;*/ end
      FUNCT3_BRANCH_GEU:  begin take_branch = result_equal_zero; /*break;*/ end
      default:                take_branch = 'x; /*break;*/
    endcase
    // clang-format on
  end
endmodule

`endif // CONTROL_TRANSFER_H
