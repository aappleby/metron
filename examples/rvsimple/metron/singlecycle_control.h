// RISC-V SiMPLE SV -- single-cycle controller
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

#ifndef SINGLECYCLE_CONTROL_H
#define SINGLECYCLE_CONTROL_H

#include "config.h"
#include "constants.h"
#include "metron/tools/metron_tools.h"

class singlecycle_control {
 public:
  logic<7> inst_opcode;
  logic<1> take_branch;
  logic<1> pc_write_enable;
  logic<1> regfile_write_enable;
  logic<1> alu_operand_a_select;
  logic<1> alu_operand_b_select;
  logic<2> alu_op_type;
  logic<1> data_mem_read_enable;
  logic<1> data_mem_write_enable;
  logic<3> reg_writeback_select;
  logic<2> next_pc_select;

  void tock_next_pc_select() {
    using namespace rv_constants;
    // clang-format off
    switch (inst_opcode) {
      case OPCODE_BRANCH: next_pc_select = take_branch ? CTL_PC_PC_IMM : CTL_PC_PC4; break;
      case OPCODE_JALR:   next_pc_select = CTL_PC_RS1_IMM; break;
      case OPCODE_JAL:    next_pc_select = CTL_PC_PC_IMM; break;
      default:            next_pc_select = CTL_PC_PC4; break;
    }
    // clang-format on
  }

  void tock_pc_write_enable() { pc_write_enable = 0b1; }

  void tock_regfile_write_enable() {
    using namespace rv_constants;
    // clang-format off
    switch (inst_opcode) {
      case OPCODE_MISC_MEM: regfile_write_enable = 0; break;
      case OPCODE_STORE:    regfile_write_enable = 0; break;
      case OPCODE_BRANCH:   regfile_write_enable = 0; break;
      case OPCODE_LOAD:     regfile_write_enable = 1; break;
      case OPCODE_OP_IMM:   regfile_write_enable = 1; break;
      case OPCODE_AUIPC:    regfile_write_enable = 1; break;
      case OPCODE_OP:       regfile_write_enable = 1; break;
      case OPCODE_LUI:      regfile_write_enable = 1; break;
      case OPCODE_JALR:     regfile_write_enable = 1; break;
      case OPCODE_JAL:      regfile_write_enable = 1; break;
      default:              regfile_write_enable = DONTCARE; break;
    }
    // clang-format on
  }

  void tock_alu_operand_a_select() {
    using namespace rv_constants;

    // clang-format off
    switch (inst_opcode) {
      case OPCODE_AUIPC:    alu_operand_a_select = CTL_ALU_A_PC; break;
      case OPCODE_JAL:      alu_operand_a_select = CTL_ALU_A_PC; break;

      case OPCODE_OP:       alu_operand_a_select = CTL_ALU_A_RS1; break;
      case OPCODE_LUI:      alu_operand_a_select = CTL_ALU_A_RS1; break;
      case OPCODE_BRANCH:   alu_operand_a_select = CTL_ALU_A_RS1; break;

      case OPCODE_LOAD:     alu_operand_a_select = CTL_ALU_A_RS1; break;
      case OPCODE_STORE:    alu_operand_a_select = CTL_ALU_A_RS1; break;
      case OPCODE_OP_IMM:   alu_operand_a_select = CTL_ALU_A_RS1; break;
      case OPCODE_JALR:     alu_operand_a_select = CTL_ALU_A_RS1; break;
      default:              alu_operand_a_select = DONTCARE; break;
    }
    // clang-format on
  }

  void tock_alu_operand_b_select() {
    using namespace rv_constants;

    // clang-format off
    switch (inst_opcode) {
      case OPCODE_AUIPC:    alu_operand_b_select = CTL_ALU_B_IMM; break;
      case OPCODE_JAL:      alu_operand_b_select = CTL_ALU_B_IMM; break;

      case OPCODE_OP:       alu_operand_b_select = CTL_ALU_B_RS2; break;
      case OPCODE_LUI:      alu_operand_b_select = CTL_ALU_B_RS2; break;
      case OPCODE_BRANCH:   alu_operand_b_select = CTL_ALU_B_RS2; break;

      case OPCODE_LOAD:     alu_operand_b_select = CTL_ALU_B_IMM; break;
      case OPCODE_STORE:    alu_operand_b_select = CTL_ALU_B_IMM; break;
      case OPCODE_OP_IMM:   alu_operand_b_select = CTL_ALU_B_IMM; break;
      case OPCODE_JALR:     alu_operand_b_select = CTL_ALU_B_IMM; break;
      default:              alu_operand_b_select = DONTCARE; break;
    }
    // clang-format on
  }

  void tock_alu_op_type() {
    using namespace rv_constants;

    // clang-format off
    switch (inst_opcode) {
      case OPCODE_AUIPC:    alu_op_type = CTL_ALU_ADD; break;
      case OPCODE_JAL:      alu_op_type = CTL_ALU_ADD; break;

      case OPCODE_OP:       alu_op_type = CTL_ALU_OP; break;
      case OPCODE_BRANCH:   alu_op_type = CTL_ALU_BRANCH; break;

      case OPCODE_LOAD:     alu_op_type = CTL_ALU_ADD; break;
      case OPCODE_STORE:    alu_op_type = CTL_ALU_ADD; break;
      case OPCODE_OP_IMM:   alu_op_type = CTL_ALU_OP_IMM; break;
      case OPCODE_JALR:     alu_op_type = CTL_ALU_ADD; break;
      default:              alu_op_type = DONTCARE; break;
    }
    // clang-format on
  }

  void tock_data_mem_read_enable() {
    using namespace rv_constants;
    data_mem_read_enable = inst_opcode == OPCODE_LOAD;
  }

  void tock_data_mem_write_enable() {
    using namespace rv_constants;
    data_mem_write_enable = inst_opcode == OPCODE_STORE;
  }

  void tock_reg_writeback_select() {
    using namespace rv_constants;

    // clang-format off
    switch (inst_opcode) {
      case OPCODE_OP_IMM:   reg_writeback_select = CTL_WRITEBACK_ALU; break;
      case OPCODE_AUIPC:    reg_writeback_select = CTL_WRITEBACK_ALU; break;
      case OPCODE_OP:       reg_writeback_select = CTL_WRITEBACK_ALU; break;
      case OPCODE_LUI:      reg_writeback_select = CTL_WRITEBACK_IMM; break;
      case OPCODE_JALR:     reg_writeback_select = CTL_WRITEBACK_PC4; break;
      case OPCODE_JAL:      reg_writeback_select = CTL_WRITEBACK_PC4; break;
      case OPCODE_LOAD:     reg_writeback_select = CTL_WRITEBACK_DATA; break;
      default:              reg_writeback_select = DONTCARE; break;
    }
    // clang-format on
  }
};

#endif // SINGLECYCLE_CONTROL_H
