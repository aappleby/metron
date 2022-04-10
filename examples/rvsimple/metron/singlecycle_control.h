// RISC-V SiMPLE SV -- single-cycle controller
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

#ifndef RVSIMPLE_SINGLECYCLE_CONTROL_H
#define RVSIMPLE_SINGLECYCLE_CONTROL_H

#include "config.h"
#include "constants.h"
#include "metron_tools.h"

class singlecycle_control {
 public:

   logic<7> inst_opcode;

  logic<2> next_pc_select(logic<1> take_branch) const {
    using namespace rv_constants;
    logic<2> result;
    switch (inst_opcode) {
      case OPCODE_BRANCH: result = take_branch ? CTL_PC_PC_IMM : CTL_PC_PC4; break;
      case OPCODE_JALR:   result = CTL_PC_RS1_IMM; break;
      case OPCODE_JAL:    result = CTL_PC_PC_IMM; break;
      default:            result = CTL_PC_PC4; break;
    }
    return result;
  }

  logic<1> pc_write_enable() const {
    return 0b1;
  }

  logic<1> regfile_write_enable() const {
    using namespace rv_constants;
    logic<1> result;
    switch (inst_opcode) {
      case OPCODE_MISC_MEM: result = 0; break;
      case OPCODE_STORE:    result = 0; break;
      case OPCODE_BRANCH:   result = 0; break;
      case OPCODE_LOAD:     result = 1; break;
      case OPCODE_OP_IMM:   result = 1; break;
      case OPCODE_AUIPC:    result = 1; break;
      case OPCODE_OP:       result = 1; break;
      case OPCODE_LUI:      result = 1; break;
      case OPCODE_JALR:     result = 1; break;
      case OPCODE_JAL:      result = 1; break;
      default:              result = b1(DONTCARE); break;
    }
    return result;
  }

  logic<1> alu_operand_a_select() const {
    using namespace rv_constants;

    logic<1> result;
    switch (inst_opcode) {
      case OPCODE_AUIPC:    result = CTL_ALU_A_PC; break;
      case OPCODE_JAL:      result = CTL_ALU_A_PC; break;

      case OPCODE_OP:       result = CTL_ALU_A_RS1; break;
      case OPCODE_LUI:      result = CTL_ALU_A_RS1; break;
      case OPCODE_BRANCH:   result = CTL_ALU_A_RS1; break;

      case OPCODE_LOAD:     result = CTL_ALU_A_RS1; break;
      case OPCODE_STORE:    result = CTL_ALU_A_RS1; break;
      case OPCODE_OP_IMM:   result = CTL_ALU_A_RS1; break;
      case OPCODE_JALR:     result = CTL_ALU_A_RS1; break;
      default:              result = b1(DONTCARE); break;
    }
    return result;
  }

  logic<1> alu_operand_b_select;
  void tock_alu_operand_b_select() {
    using namespace rv_constants;
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
      default:              alu_operand_b_select = b1(DONTCARE); break;
    }
  }

  logic<2> alu_op_type2;
  void tock_alu_op_type2() {
    using namespace rv_constants;
    switch (inst_opcode) {
      case OPCODE_AUIPC:    alu_op_type2 = CTL_ALU_ADD; break;
      case OPCODE_JAL:      alu_op_type2 = CTL_ALU_ADD; break;

      case OPCODE_OP:       alu_op_type2 = CTL_ALU_OP; break;
      case OPCODE_BRANCH:   alu_op_type2 = CTL_ALU_BRANCH; break;

      case OPCODE_LOAD:     alu_op_type2 = CTL_ALU_ADD; break;
      case OPCODE_STORE:    alu_op_type2 = CTL_ALU_ADD; break;
      case OPCODE_OP_IMM:   alu_op_type2 = CTL_ALU_OP_IMM; break;
      case OPCODE_JALR:     alu_op_type2 = CTL_ALU_ADD; break;
      default:              alu_op_type2 = b2(DONTCARE); break;
    }
  }

  logic<1> data_mem_read_enable;
  void tock_data_mem_read_enable() {
    using namespace rv_constants;
    data_mem_read_enable = inst_opcode == OPCODE_LOAD;
  }

  logic<1> data_mem_write_enable;
  void tock_data_mem_write_enable() {
      using namespace rv_constants;
      data_mem_write_enable = inst_opcode == OPCODE_STORE;
  }

  logic<3> reg_writeback_select;
  void tock_reg_writeback_select() {
    using namespace rv_constants;
    switch (inst_opcode) {
      case OPCODE_OP_IMM:   reg_writeback_select = CTL_WRITEBACK_ALU; break;
      case OPCODE_AUIPC:    reg_writeback_select = CTL_WRITEBACK_ALU; break;
      case OPCODE_OP:       reg_writeback_select = CTL_WRITEBACK_ALU; break;
      case OPCODE_LUI:      reg_writeback_select = CTL_WRITEBACK_IMM; break;
      case OPCODE_JALR:     reg_writeback_select = CTL_WRITEBACK_PC4; break;
      case OPCODE_JAL:      reg_writeback_select = CTL_WRITEBACK_PC4; break;
      case OPCODE_LOAD:     reg_writeback_select = CTL_WRITEBACK_DATA; break;
      default:              reg_writeback_select = b3(DONTCARE); break;
    }
  }
};

#endif  // RVSIMPLE_SINGLECYCLE_CONTROL_H
