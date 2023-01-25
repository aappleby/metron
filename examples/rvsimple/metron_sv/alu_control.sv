// RISC-V SiMPLE SV -- ALU controller module
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

`ifndef ALU_CONTROL_H
`define ALU_CONTROL_H

`include "config.sv"
`include "constants.sv"
`include "metron_tools.sv"

module alu_control (
  // input signals
  input logic[1:0] alu_op_type,
  input logic[2:0] inst_funct3,
  input logic[6:0] inst_funct7,
  // output signals
  output logic[4:0] alu_function
);
 /*public:*/

 /*private:*/
  logic[4:0] default_funct;
  logic[4:0] secondary_funct;
  logic[4:0] op_funct;
  logic[4:0] op_imm_funct;
  logic[4:0] branch_funct;

 /*public:*/
  // clang-format off
  always_comb begin : tock_alu_function
    import rv_constants::*;

    case (inst_funct3)
      FUNCT3_ALU_ADD_SUB: default_funct = ALU_ADD;
      FUNCT3_ALU_SLL:     default_funct = ALU_SLL;
      FUNCT3_ALU_SLT:     default_funct = ALU_SLT;
      FUNCT3_ALU_SLTU:    default_funct = ALU_SLTU;
      FUNCT3_ALU_XOR:     default_funct = ALU_XOR;
      FUNCT3_ALU_SHIFTR:  default_funct = ALU_SRL;
      FUNCT3_ALU_OR:      default_funct = ALU_OR;
      FUNCT3_ALU_AND:     default_funct = ALU_AND;
      default:                 default_funct = 'x;
    endcase

    case (inst_funct3)
      FUNCT3_ALU_ADD_SUB: secondary_funct = ALU_SUB;
      FUNCT3_ALU_SHIFTR:  secondary_funct = ALU_SRA;
      default:                 secondary_funct = 'x;
    endcase

    if (inst_funct7[5])
      op_funct = secondary_funct;
    else
      op_funct = default_funct;

    if (inst_funct7[5] && 2'(inst_funct3) == 2'b01)
      op_imm_funct = secondary_funct;
    else
      op_imm_funct = default_funct;

    case (inst_funct3)
      FUNCT3_BRANCH_EQ:  branch_funct = ALU_SEQ;
      FUNCT3_BRANCH_NE:  branch_funct = ALU_SEQ;
      FUNCT3_BRANCH_LT:  branch_funct = ALU_SLT;
      FUNCT3_BRANCH_GE:  branch_funct = ALU_SLT;
      FUNCT3_BRANCH_LTU: branch_funct = ALU_SLTU;
      FUNCT3_BRANCH_GEU: branch_funct = ALU_SLTU;
      default:                branch_funct = 'x;
    endcase

    case (alu_op_type)
      CTL_ALU_ADD:    alu_function = ALU_ADD;
      CTL_ALU_OP:     alu_function = op_funct;
      CTL_ALU_OP_IMM: alu_function = op_imm_funct;
      CTL_ALU_BRANCH: alu_function = branch_funct;
      default:             alu_function = 'x;
    endcase
  end
  // clang-format on
endmodule

`endif // ALU_CONTROL_H
