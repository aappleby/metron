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
  logic<3> inst_funct3;
  logic<1> alu_result_equal_zero;
  logic<7> inst_funct7;

  logic<5> alu_function;
  logic<1> pc_write_enable;
  logic<1> regfile_write_enable;
  logic<1> alu_operand_a_select;
  logic<1> alu_operand_b_select;
  logic<1> data_mem_read_enable;
  logic<1> data_mem_write_enable;
  logic<3> reg_writeback_select;
  logic<2> next_pc_select;

  void tock1() {
    control.inst_opcode = inst_opcode;

    control.tock_alu_op_type2();
    alu_ctrl.alu_op_type = control.alu_op_type2;
    alu_ctrl.inst_funct3 = inst_funct3;
    alu_ctrl.inst_funct7 = inst_funct7;

    alu_ctrl.tock_alu_function();
    alu_function = alu_ctrl.alu_function;
    alu_operand_a_select = control.alu_operand_a_select();
    control.tock_alu_operand_b_select();
    alu_operand_b_select = control.alu_operand_b_select;

    control.tock_data_mem_read_enable();
    data_mem_read_enable = control.data_mem_read_enable;

    control.tock_data_mem_write_enable();
    data_mem_write_enable = control.data_mem_write_enable;
  }


  void tock2() {
    pc_write_enable = control.pc_write_enable();
    regfile_write_enable = control.regfile_write_enable();

    control.tock_reg_writeback_select();
    reg_writeback_select = control.reg_writeback_select;

    transfer.result_equal_zero = alu_result_equal_zero;
    transfer.inst_funct3 = inst_funct3;
    transfer.tock_take_branch();
    logic<1> take_branch = transfer.take_branch;
    next_pc_select = control.next_pc_select(take_branch);
  }

  //----------------------------------------

 private:
  singlecycle_control control;
  control_transfer transfer;
  alu_control alu_ctrl;
};

#endif  // RVSIMPLE_SINGLECYCLE_CTLPATH_H
