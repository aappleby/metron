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
  //logic<1> pc_write_enable;
  //logic<1> regfile_write_enable;
  //logic<1> alu_operand_a_select;
  //logic<1> alu_operand_b_select;
  logic<2> alu_op_type;
  logic<1> data_mem_read_enable;
  logic<1> data_mem_write_enable;
  logic<3> reg_writeback_select;
  logic<2> next_pc_select;

  void tock_next_pc_select(logic<7> inst_opcode, logic<1> take_branch) {
    using namespace rv_constants;
    switch (inst_opcode) {
      case OPCODE_BRANCH: next_pc_select = take_branch ? CTL_PC_PC_IMM : CTL_PC_PC4; break;
      case OPCODE_JALR:   next_pc_select = CTL_PC_RS1_IMM; break;
      case OPCODE_JAL:    next_pc_select = CTL_PC_PC_IMM; break;
      default:            next_pc_select = CTL_PC_PC4; break;
    }
  }

  logic<1> pc_write_enable(logic<7> inst_opcode) const {
    return 0b1;
  }

  logic<1> regfile_write_enable(logic<7> inst_opcode) const {
    using namespace rv_constants;
    switch (inst_opcode) {
      case OPCODE_MISC_MEM: return 0;
      case OPCODE_STORE:    return 0;
      case OPCODE_BRANCH:   return 0;
      case OPCODE_LOAD:     return 1;
      case OPCODE_OP_IMM:   return 1;
      case OPCODE_AUIPC:    return 1;
      case OPCODE_OP:       return 1;
      case OPCODE_LUI:      return 1;
      case OPCODE_JALR:     return 1;
      case OPCODE_JAL:      return 1;
      default:              return b1(DONTCARE);
    }
  }

  logic<1> alu_operand_a_select(logic<7> inst_opcode) const {
    using namespace rv_constants;

    switch (inst_opcode) {
      case OPCODE_LOAD:     return CTL_ALU_A_RS1;
      case OPCODE_MISC_MEM: return b1(DONTCARE);
      case OPCODE_OP_IMM:   return CTL_ALU_A_RS1;
      case OPCODE_AUIPC:    return CTL_ALU_A_PC;
      case OPCODE_STORE:    return CTL_ALU_A_RS1;
      case OPCODE_OP:       return CTL_ALU_A_RS1;
      case OPCODE_LUI:      return CTL_ALU_A_RS1;
      case OPCODE_BRANCH:   return CTL_ALU_A_RS1;
      case OPCODE_JALR:     return CTL_ALU_A_RS1;
      case OPCODE_JAL:      return CTL_ALU_A_PC;
      default:              return b1(DONTCARE);
    }
  }

  logic<1> alu_operand_b_select(logic<7> inst_opcode) const {
    using namespace rv_constants;
    switch (inst_opcode) {
      case OPCODE_LOAD:     return CTL_ALU_B_IMM;
      case OPCODE_MISC_MEM: return b1(DONTCARE);
      case OPCODE_OP_IMM:   return CTL_ALU_B_IMM;
      case OPCODE_AUIPC:    return CTL_ALU_B_IMM;
      case OPCODE_STORE:    return CTL_ALU_B_IMM;
      case OPCODE_OP:       return CTL_ALU_B_RS2;
      case OPCODE_LUI:      return CTL_ALU_B_RS2;
      case OPCODE_BRANCH:   return CTL_ALU_B_RS2;
      case OPCODE_JALR:     return CTL_ALU_B_IMM;
      case OPCODE_JAL:      return CTL_ALU_B_IMM;
      default:              return b1(DONTCARE);
    }
  }

  logic<2> alu_op_type2(logic<7> inst_opcode) const {
    using namespace rv_constants;
    switch (inst_opcode) {
      case OPCODE_LOAD: return CTL_ALU_ADD;
      case OPCODE_MISC_MEM: return b2(DONTCARE);
      case OPCODE_OP_IMM: return CTL_ALU_OP_IMM;
      case OPCODE_AUIPC: return CTL_ALU_ADD;
      case OPCODE_STORE: return CTL_ALU_ADD;
      case OPCODE_OP: return CTL_ALU_OP;
      case OPCODE_LUI: return b2(DONTCARE);
      case OPCODE_BRANCH: return CTL_ALU_BRANCH;
      case OPCODE_JALR: return CTL_ALU_ADD;
      case OPCODE_JAL: return CTL_ALU_ADD;
      default: return b2(DONTCARE);
    }
  }


  void tock_decode(logic<7> inst_opcode) {
    using namespace rv_constants;

    alu_op_type             = b2(DONTCARE);
    data_mem_read_enable    = b1(0b0);
    data_mem_write_enable   = b1(0b0);
    reg_writeback_select    = b3(DONTCARE);

    switch (inst_opcode) {
      case OPCODE_LOAD:
      {
        alu_op_type = CTL_ALU_ADD;
        data_mem_read_enable = b1(1);
        reg_writeback_select = CTL_WRITEBACK_DATA;
        break;
      }

      case OPCODE_MISC_MEM:
      {
        // Fence - ignore
        break;
      }

      case OPCODE_OP_IMM:
      {
        alu_op_type = CTL_ALU_OP_IMM;
        reg_writeback_select = CTL_WRITEBACK_ALU;
        break;
      }

      case OPCODE_AUIPC:
      {
        alu_op_type = CTL_ALU_ADD;
        reg_writeback_select = CTL_WRITEBACK_ALU;
        break;
      }

      case OPCODE_STORE:
      {
        alu_op_type = CTL_ALU_ADD;
        data_mem_write_enable = b1(1);
        break;
      }

      case OPCODE_OP:
      {
        reg_writeback_select = CTL_WRITEBACK_ALU;
        alu_op_type = CTL_ALU_OP;
        break;
      }

      case OPCODE_LUI:
      {
        reg_writeback_select = CTL_WRITEBACK_IMM;
        break;
      }

      case OPCODE_BRANCH:
      {
        alu_op_type = CTL_ALU_BRANCH;
        break;
      }

      case OPCODE_JALR:
      {
        alu_op_type = CTL_ALU_ADD;
        reg_writeback_select = CTL_WRITEBACK_PC4;
        break;
      }

      case OPCODE_JAL:
      {
        alu_op_type = CTL_ALU_ADD;
        reg_writeback_select = CTL_WRITEBACK_PC4;
        break;
      }

      default:
      {
        data_mem_read_enable = b1(DONTCARE);
        data_mem_write_enable = b1(DONTCARE);
        break;
      }
    }
  }
};

#endif  // RVSIMPLE_SINGLECYCLE_CONTROL_H
