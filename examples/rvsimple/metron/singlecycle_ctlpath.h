// RISC-V SiMPLE SV -- control path
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

#ifndef RVSIMPLE_SINGLECYCLE_CTLPATH_H
#define RVSIMPLE_SINGLECYCLE_CTLPATH_H

#include "alu_control.h"
#include "config.h"
#include "constants.h"
#include "control_transfer.h"
#include "metron_tools.h"
#include "singlecycle_control.h"

class singlecycle_ctlpath {
 public:

   logic<7> inst_opcode;

  logic<5> alu_function(logic<3> inst_funct3, logic<7> inst_funct7) const {
    return alu_ctrl.alu_function(control.alu_op_type2(inst_opcode), inst_funct3, inst_funct7);
  }

  logic<1> pc_write_enable() const {
    return control.pc_write_enable(inst_opcode);
  }

  logic<1> regfile_write_enable() const {
    return control.regfile_write_enable(inst_opcode);
  }

  logic<1> alu_operand_a_select() const {
    return control.alu_operand_a_select(inst_opcode);
  }

  logic<1> alu_operand_b_select() const {
    return control.alu_operand_b_select(inst_opcode);
  }

  logic<1> data_mem_read_enable() const {
    return control.data_mem_read_enable(inst_opcode);
  }

  logic<1> data_mem_write_enable() const {
    return control.data_mem_write_enable(inst_opcode);
  }

  logic<3> reg_writeback_select() const {
    return control.reg_writeback_select(inst_opcode);
  }

  logic<2> next_pc_select(logic<3> inst_funct3, logic<1> alu_result_equal_zero) const {
    logic<1> take_branch = transfer.take_branch(alu_result_equal_zero, inst_funct3);
    return control.next_pc_select(inst_opcode, take_branch);
  }

  //----------------------------------------

 private:
  singlecycle_control control;
  control_transfer transfer;
  alu_control alu_ctrl;
};

#endif  // RVSIMPLE_SINGLECYCLE_CTLPATH_H
