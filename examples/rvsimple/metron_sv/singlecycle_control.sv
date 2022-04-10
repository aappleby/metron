// RISC-V SiMPLE SV -- single-cycle controller
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

`ifndef RVSIMPLE_SINGLECYCLE_CONTROL_H
`define RVSIMPLE_SINGLECYCLE_CONTROL_H

`include "config.sv"
`include "constants.sv"
`include "metron_tools.sv"

module singlecycle_control
(
  input logic clock,
  input logic[6:0] inst_opcode,
  input logic take_branch,
  output logic pc_write_enable,
  output logic regfile_write_enable,
  output logic alu_operand_a_select,
  output logic alu_operand_b_select,
  output logic[1:0] alu_op_type,
  output logic data_mem_read_enable,
  output logic data_mem_write_enable,
  output logic[2:0] reg_writeback_select,
  output logic[1:0] next_pc_select
);

  always_comb begin /*tock_next_pc_select*/
    import rv_constants::*;
    // clang-format off
    case (inst_opcode)
      OPCODE_BRANCH: next_pc_select = take_branch ? CTL_PC_PC_IMM : CTL_PC_PC4;
      OPCODE_JALR:   next_pc_select = CTL_PC_RS1_IMM;
      OPCODE_JAL:    next_pc_select = CTL_PC_PC_IMM;
      default:            next_pc_select = CTL_PC_PC4;
    endcase
    // clang-format on
  end

  always_comb begin /*tock_pc_write_enable*/ pc_write_enable = 1'b1; end

  always_comb begin /*tock_regfile_write_enable*/
    import rv_constants::*;
    // clang-format off
    case (inst_opcode)
      OPCODE_MISC_MEM: regfile_write_enable = 0;
      OPCODE_STORE:    regfile_write_enable = 0;
      OPCODE_BRANCH:   regfile_write_enable = 0;
      OPCODE_LOAD:     regfile_write_enable = 1;
      OPCODE_OP_IMM:   regfile_write_enable = 1;
      OPCODE_AUIPC:    regfile_write_enable = 1;
      OPCODE_OP:       regfile_write_enable = 1;
      OPCODE_LUI:      regfile_write_enable = 1;
      OPCODE_JALR:     regfile_write_enable = 1;
      OPCODE_JAL:      regfile_write_enable = 1;
      default:              regfile_write_enable = 1'bx;
    endcase
    // clang-format on
  end

  always_comb begin /*tock_alu_operand_a_select*/
    import rv_constants::*;

    // clang-format off
    case (inst_opcode)
      OPCODE_AUIPC:    alu_operand_a_select = CTL_ALU_A_PC;
      OPCODE_JAL:      alu_operand_a_select = CTL_ALU_A_PC;

      OPCODE_OP:       alu_operand_a_select = CTL_ALU_A_RS1;
      OPCODE_LUI:      alu_operand_a_select = CTL_ALU_A_RS1;
      OPCODE_BRANCH:   alu_operand_a_select = CTL_ALU_A_RS1;

      OPCODE_LOAD:     alu_operand_a_select = CTL_ALU_A_RS1;
      OPCODE_STORE:    alu_operand_a_select = CTL_ALU_A_RS1;
      OPCODE_OP_IMM:   alu_operand_a_select = CTL_ALU_A_RS1;
      OPCODE_JALR:     alu_operand_a_select = CTL_ALU_A_RS1;
      default:              alu_operand_a_select = 1'bx;
    endcase
    // clang-format on
  end

  always_comb begin /*tock_alu_operand_b_select*/
    import rv_constants::*;

    // clang-format off
    case (inst_opcode)
      OPCODE_AUIPC:    alu_operand_b_select = CTL_ALU_B_IMM;
      OPCODE_JAL:      alu_operand_b_select = CTL_ALU_B_IMM;

      OPCODE_OP:       alu_operand_b_select = CTL_ALU_B_RS2;
      OPCODE_LUI:      alu_operand_b_select = CTL_ALU_B_RS2;
      OPCODE_BRANCH:   alu_operand_b_select = CTL_ALU_B_RS2;

      OPCODE_LOAD:     alu_operand_b_select = CTL_ALU_B_IMM;
      OPCODE_STORE:    alu_operand_b_select = CTL_ALU_B_IMM;
      OPCODE_OP_IMM:   alu_operand_b_select = CTL_ALU_B_IMM;
      OPCODE_JALR:     alu_operand_b_select = CTL_ALU_B_IMM;
      default:              alu_operand_b_select = 1'bx;
    endcase
    // clang-format on
  end

  always_comb begin /*tock_alu_op_type*/
    import rv_constants::*;

    // clang-format off
    case (inst_opcode)
      OPCODE_AUIPC:    alu_op_type = CTL_ALU_ADD;
      OPCODE_JAL:      alu_op_type = CTL_ALU_ADD;

      OPCODE_OP:       alu_op_type = CTL_ALU_OP;
      OPCODE_BRANCH:   alu_op_type = CTL_ALU_BRANCH;

      OPCODE_LOAD:     alu_op_type = CTL_ALU_ADD;
      OPCODE_STORE:    alu_op_type = CTL_ALU_ADD;
      OPCODE_OP_IMM:   alu_op_type = CTL_ALU_OP_IMM;
      OPCODE_JALR:     alu_op_type = CTL_ALU_ADD;
      default:              alu_op_type = 2'bx;
    endcase
    // clang-format on
  end

  always_comb begin /*tock_data_mem_read_enable*/
    import rv_constants::*;
    data_mem_read_enable = inst_opcode == OPCODE_LOAD;
  end

  always_comb begin /*tock_data_mem_write_enable*/
    import rv_constants::*;
    data_mem_write_enable = inst_opcode == OPCODE_STORE;
  end

  always_comb begin /*tock_reg_writeback_select*/
    import rv_constants::*;

    // clang-format off
    case (inst_opcode)
      OPCODE_OP_IMM:   reg_writeback_select = CTL_WRITEBACK_ALU;
      OPCODE_AUIPC:    reg_writeback_select = CTL_WRITEBACK_ALU;
      OPCODE_OP:       reg_writeback_select = CTL_WRITEBACK_ALU;
      OPCODE_LUI:      reg_writeback_select = CTL_WRITEBACK_IMM;
      OPCODE_JALR:     reg_writeback_select = CTL_WRITEBACK_PC4;
      OPCODE_JAL:      reg_writeback_select = CTL_WRITEBACK_PC4;
      OPCODE_LOAD:     reg_writeback_select = CTL_WRITEBACK_DATA;
      default:              reg_writeback_select = 3'bx;
    endcase
    // clang-format on
  end
endmodule;

`endif  // RVSIMPLE_SINGLECYCLE_CONTROL_H

