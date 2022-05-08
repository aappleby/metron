// RISC-V SiMPLE SV -- control path
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

`ifndef SINGLECYCLE_CTLPATH_H
`define SINGLECYCLE_CTLPATH_H

`include "alu_control.sv"
`include "config.sv"
`include "constants.sv"
`include "control_transfer.sv"
`include "metron_tools.sv"
`include "singlecycle_control.sv"

module singlecycle_ctlpath
(
  input logic[6:0] inst_opcode,
  input logic[2:0] inst_funct3,
  input logic[6:0] inst_funct7,
  input logic alu_result_equal_zero,
  output logic pc_write_enable,
  output logic regfile_write_enable,
  output logic alu_operand_a_select,
  output logic alu_operand_b_select,
  output logic data_mem_read_enable,
  output logic data_mem_write_enable,
  output logic[2:0] reg_writeback_select,
  output logic[4:0] alu_function,
  output logic[1:0] next_pc_select
);
 /*public:*/


  task automatic tock_alu_function();
    control_inst_opcode = inst_opcode;
    /*control.tock_alu_op_type*/;
    alu_ctrl_alu_op_type = control_alu_op_type;
    alu_ctrl_inst_funct3 = inst_funct3;
    alu_ctrl_inst_funct7 = inst_funct7;
    /*alu_ctrl.tock_alu_function*/;
    alu_function = alu_ctrl_alu_function;
  endtask
  always_comb tock_alu_function();

  task automatic tock_alu_operand_select();
    /*control.tock_alu_operand_a_select*/;
    /*control.tock_alu_operand_b_select*/;
    alu_operand_a_select = control_alu_operand_a_select;
    alu_operand_b_select = control_alu_operand_b_select;
  endtask
  always_comb tock_alu_operand_select();

  task automatic tock_data_mem_enable();
    /*control.tock_data_mem_read_enable*/;
    /*control.tock_data_mem_write_enable*/;
    data_mem_read_enable = control_data_mem_read_enable;
    data_mem_write_enable = control_data_mem_write_enable;
  endtask
  always_comb tock_data_mem_enable();

  task automatic tock_pc_write_enable();
    /*control.tock_pc_write_enable*/;
    pc_write_enable = control_pc_write_enable;
  endtask
  always_comb tock_pc_write_enable();

  task automatic tock_regfile_write_enable();
    /*control.tock_regfile_write_enable*/;
    regfile_write_enable = control_regfile_write_enable;
  endtask
  always_comb tock_regfile_write_enable();

  task automatic tock_reg_writeback_select();
    /*control.tock_reg_writeback_select*/;
    reg_writeback_select = control_reg_writeback_select;
  endtask
  always_comb tock_reg_writeback_select();

  task automatic tock_next_pc_select();
    transfer_result_equal_zero = alu_result_equal_zero;
    transfer_inst_funct3 = inst_funct3;
    /*transfer.tock_take_branch*/;
    control_take_branch = transfer_take_branch;
    /*control.tock_next_pc_select*/;
    next_pc_select = control_next_pc_select;
  endtask
  always_comb tock_next_pc_select();

  //----------------------------------------

 /*private:*/
  singlecycle_control control(
    .inst_opcode(control_inst_opcode),
    .take_branch(control_take_branch),
    .pc_write_enable(control_pc_write_enable),
    .regfile_write_enable(control_regfile_write_enable),
    .alu_operand_a_select(control_alu_operand_a_select),
    .alu_operand_b_select(control_alu_operand_b_select),
    .alu_op_type(control_alu_op_type),
    .data_mem_read_enable(control_data_mem_read_enable),
    .data_mem_write_enable(control_data_mem_write_enable),
    .reg_writeback_select(control_reg_writeback_select),
    .next_pc_select(control_next_pc_select)
  );
  logic[6:0] control_inst_opcode;
  logic control_take_branch;
  logic control_pc_write_enable;
  logic control_regfile_write_enable;
  logic control_alu_operand_a_select;
  logic control_alu_operand_b_select;
  logic[1:0] control_alu_op_type;
  logic control_data_mem_read_enable;
  logic control_data_mem_write_enable;
  logic[2:0] control_reg_writeback_select;
  logic[1:0] control_next_pc_select;

  control_transfer transfer(
    .result_equal_zero(transfer_result_equal_zero),
    .inst_funct3(transfer_inst_funct3),
    .take_branch(transfer_take_branch)
  );
  logic transfer_result_equal_zero;
  logic[2:0] transfer_inst_funct3;
  logic transfer_take_branch;

  alu_control alu_ctrl(
    .alu_op_type(alu_ctrl_alu_op_type),
    .inst_funct3(alu_ctrl_inst_funct3),
    .inst_funct7(alu_ctrl_inst_funct7),
    .alu_function(alu_ctrl_alu_function)
  );
  logic[1:0] alu_ctrl_alu_op_type;
  logic[2:0] alu_ctrl_inst_funct3;
  logic[6:0] alu_ctrl_inst_funct7;
  logic[4:0] alu_ctrl_alu_function;

endmodule

`endif // SINGLECYCLE_CTLPATH_H
