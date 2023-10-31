// RISC-V SiMPLE SV -- ALU controller module
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

`ifndef ALU_CONTROL_H
`define ALU_CONTROL_H

`include "config.sv"
`include "constants.sv"
`include "metron/metron_tools.sv"

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
      FUNCT3_ALU_ADD_SUB:  begin default_funct = ALU_ADD; /*break;*/ end
      FUNCT3_ALU_SLL:      begin default_funct = ALU_SLL; /*break;*/ end
      FUNCT3_ALU_SLT:      begin default_funct = ALU_SLT; /*break;*/ end
      FUNCT3_ALU_SLTU:     begin default_funct = ALU_SLTU; /*break;*/ end
      FUNCT3_ALU_XOR:      begin default_funct = ALU_XOR; /*break;*/ end
      FUNCT3_ALU_SHIFTR:   begin default_funct = ALU_SRL; /*break;*/ end
      FUNCT3_ALU_OR:       begin default_funct = ALU_OR; /*break;*/ end
      FUNCT3_ALU_AND:      begin default_funct = ALU_AND; /*break;*/ end
      default:                 default_funct = 'x; /*break;*/
    endcase

    case (inst_funct3)
      FUNCT3_ALU_ADD_SUB:  begin secondary_funct = ALU_SUB; /*break;*/ end
      FUNCT3_ALU_SHIFTR:   begin secondary_funct = ALU_SRA; /*break;*/ end
      default:                 secondary_funct = 'x; /*break;*/
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
      FUNCT3_BRANCH_EQ:   begin branch_funct = ALU_SEQ; /*break;*/ end
      FUNCT3_BRANCH_NE:   begin branch_funct = ALU_SEQ; /*break;*/ end
      FUNCT3_BRANCH_LT:   begin branch_funct = ALU_SLT; /*break;*/ end
      FUNCT3_BRANCH_GE:   begin branch_funct = ALU_SLT; /*break;*/ end
      FUNCT3_BRANCH_LTU:  begin branch_funct = ALU_SLTU; /*break;*/ end
      FUNCT3_BRANCH_GEU:  begin branch_funct = ALU_SLTU; /*break;*/ end
      default:                branch_funct = 'x; /*break;*/
    endcase

    case (alu_op_type)
      CTL_ALU_ADD:     begin alu_function = ALU_ADD; /*break;*/ end
      CTL_ALU_OP:      begin alu_function = op_funct; /*break;*/ end
      CTL_ALU_OP_IMM:  begin alu_function = op_imm_funct; /*break;*/ end
      default:             alu_function = 'x; /*break;*/
      CTL_ALU_BRANCH:  begin alu_function = branch_funct; /*break;*/ end
    endcase
  end
  // clang-format on
endmodule

`endif // ALU_CONTROL_H
