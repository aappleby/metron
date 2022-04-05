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

  logic<2> next_pc_select(logic<7> inst_opcode, logic<1> take_branch) const {
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

  logic<1> pc_write_enable(logic<7> inst_opcode) const {
    return 0b1;
  }

  logic<1> regfile_write_enable(logic<7> inst_opcode) const {
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

  logic<1> alu_operand_a_select(logic<7> inst_opcode) const {
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

  logic<1> alu_operand_b_select(logic<7> inst_opcode) const {
    using namespace rv_constants;
    logic<1> result;
    switch (inst_opcode) {
      case OPCODE_AUIPC:    result = CTL_ALU_B_IMM; break;
      case OPCODE_JAL:      result = CTL_ALU_B_IMM; break;

      case OPCODE_OP:       result = CTL_ALU_B_RS2; break;
      case OPCODE_LUI:      result = CTL_ALU_B_RS2; break;
      case OPCODE_BRANCH:   result = CTL_ALU_B_RS2; break;

      case OPCODE_LOAD:     result = CTL_ALU_B_IMM; break;
      case OPCODE_STORE:    result = CTL_ALU_B_IMM; break;
      case OPCODE_OP_IMM:   result = CTL_ALU_B_IMM; break;
      case OPCODE_JALR:     result = CTL_ALU_B_IMM; break;
      default:              result = b1(DONTCARE); break;
    }
    return result;
  }

  logic<2> alu_op_type2(logic<7> inst_opcode) const {
    using namespace rv_constants;
    logic<2> result;
    switch (inst_opcode) {
      case OPCODE_AUIPC:    result = CTL_ALU_ADD; break;
      case OPCODE_JAL:      result = CTL_ALU_ADD; break;

      case OPCODE_OP:       result = CTL_ALU_OP; break;
      case OPCODE_BRANCH:   result = CTL_ALU_BRANCH; break;

      case OPCODE_LOAD:     result = CTL_ALU_ADD; break;
      case OPCODE_STORE:    result = CTL_ALU_ADD; break;
      case OPCODE_OP_IMM:   result = CTL_ALU_OP_IMM; break;
      case OPCODE_JALR:     result = CTL_ALU_ADD; break;
      default:              result = b2(DONTCARE); break;
    }
    return result;
  }

  logic<1> data_mem_read_enable(logic<7> inst_opcode) const {
    using namespace rv_constants;
    return inst_opcode == OPCODE_LOAD;
  }

  logic<1> data_mem_write_enable(logic<7> inst_opcode) const {
    using namespace rv_constants;
    return inst_opcode == OPCODE_STORE;
  }

  logic<3> reg_writeback_select(logic<7> inst_opcode) const {
    using namespace rv_constants;
    logic<3> result;
    switch (inst_opcode) {
      case OPCODE_OP_IMM:   result = CTL_WRITEBACK_ALU; break;
      case OPCODE_AUIPC:    result = CTL_WRITEBACK_ALU; break;
      case OPCODE_OP:       result = CTL_WRITEBACK_ALU; break;
      case OPCODE_LUI:      result = CTL_WRITEBACK_IMM; break;
      case OPCODE_JALR:     result = CTL_WRITEBACK_PC4; break;
      case OPCODE_JAL:      result = CTL_WRITEBACK_PC4; break;
      case OPCODE_LOAD:     result = CTL_WRITEBACK_DATA; break;
      default:              result = b3(DONTCARE); break;
    }
    return result;
  }
};

#endif  // RVSIMPLE_SINGLECYCLE_CONTROL_H
