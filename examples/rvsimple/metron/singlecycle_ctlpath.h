// RISC-V SiMPLE SV -- control path
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

#ifndef SINGLECYCLE_CTLPATH_H
#define SINGLECYCLE_CTLPATH_H

#include "alu_control.h"
#include "config.h"
#include "constants.h"
#include "control_transfer.h"
#include "metron/tools/metron_tools.h"
#include "singlecycle_control.h"

class singlecycle_ctlpath {
 public:
  logic<7> inst_opcode;
  logic<3> inst_funct3;
  logic<7> inst_funct7;
  logic<1> alu_result_equal_zero;

  logic<1> pc_write_enable;
  logic<1> regfile_write_enable;
  logic<1> alu_operand_a_select;
  logic<1> alu_operand_b_select;
  logic<1> data_mem_read_enable;
  logic<1> data_mem_write_enable;
  logic<3> reg_writeback_select;
  logic<5> alu_function;
  logic<2> next_pc_select;

  void tock_alu_function() {
    control.inst_opcode = inst_opcode;
    control.tock_alu_op_type();
    alu_ctrl.alu_op_type = control.alu_op_type;
    alu_ctrl.inst_funct3 = inst_funct3;
    alu_ctrl.inst_funct7 = inst_funct7;
    alu_ctrl.tock_alu_function();
    alu_function = alu_ctrl.alu_function;
  }

  void tock_alu_operand_select() {
    control.tock_alu_operand_a_select();
    control.tock_alu_operand_b_select();
    alu_operand_a_select = control.alu_operand_a_select;
    alu_operand_b_select = control.alu_operand_b_select;
  }

  void tock_data_mem_enable() {
    control.tock_data_mem_read_enable();
    control.tock_data_mem_write_enable();
    data_mem_read_enable = control.data_mem_read_enable;
    data_mem_write_enable = control.data_mem_write_enable;
  }

  void tock_pc_write_enable() {
    control.tock_pc_write_enable();
    pc_write_enable = control.pc_write_enable;
  }

  void tock_regfile_write_enable() {
    control.tock_regfile_write_enable();
    regfile_write_enable = control.regfile_write_enable;
  }

  void tock_reg_writeback_select() {
    control.tock_reg_writeback_select();
    reg_writeback_select = control.reg_writeback_select;
  }

  void tock_next_pc_select() {
    transfer.result_equal_zero = alu_result_equal_zero;
    transfer.inst_funct3 = inst_funct3;
    transfer.tock_take_branch();
    control.take_branch = transfer.take_branch;
    control.tock_next_pc_select();
    next_pc_select = control.next_pc_select;
  }

  //----------------------------------------

 private:
  singlecycle_control control;
  control_transfer transfer;
  alu_control alu_ctrl;
};

#endif // SINGLECYCLE_CTLPATH_H
