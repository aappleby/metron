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

module singlecycle_ctlpath (
  // input signals
  input logic[6:0] inst_opcode,
  input logic[2:0] inst_funct3,
  input logic[6:0] inst_funct7,
  input logic alu_result_equal_zero,
  // output signals
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


  always_comb begin : tock_alu_function
    control_inst_opcode = inst_opcode;
    alu_ctrl_alu_op_type = control_alu_op_type;
    alu_ctrl_inst_funct3 = inst_funct3;
    alu_ctrl_inst_funct7 = inst_funct7;
    alu_function = alu_ctrl_alu_function;
  end

  always_comb begin : tock_alu_operand_select
    alu_operand_a_select = control_alu_operand_a_select;
    alu_operand_b_select = control_alu_operand_b_select;
  end

  always_comb begin : tock_data_mem_enable
    data_mem_read_enable = control_data_mem_read_enable;
    data_mem_write_enable = control_data_mem_write_enable;
  end

  always_comb begin : tock_pc_write_enable
    pc_write_enable = control_pc_write_enable;
  end

  always_comb begin : tock_regfile_write_enable
    regfile_write_enable = control_regfile_write_enable;
  end

  always_comb begin : tock_reg_writeback_select
    reg_writeback_select = control_reg_writeback_select;
  end

  always_comb begin : tock_next_pc_select
    transfer_result_equal_zero = alu_result_equal_zero;
    transfer_inst_funct3 = inst_funct3;
    control_take_branch = transfer_take_branch;
    next_pc_select = control_next_pc_select;
  end

  //----------------------------------------

 /*private:*/
  singlecycle_control control(
    // Input signals
    .inst_opcode(control_inst_opcode),
    .take_branch(control_take_branch),
    // Output signals
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
    // Input signals
    .result_equal_zero(transfer_result_equal_zero),
    .inst_funct3(transfer_inst_funct3),
    // Output signals
    .take_branch(transfer_take_branch)
  );
  logic transfer_result_equal_zero;
  logic[2:0] transfer_inst_funct3;
  logic transfer_take_branch;

  alu_control alu_ctrl(
    // Input signals
    .alu_op_type(alu_ctrl_alu_op_type),
    .inst_funct3(alu_ctrl_inst_funct3),
    .inst_funct7(alu_ctrl_inst_funct7),
    // Output signals
    .alu_function(alu_ctrl_alu_function)
  );
  logic[1:0] alu_ctrl_alu_op_type;
  logic[2:0] alu_ctrl_inst_funct3;
  logic[6:0] alu_ctrl_inst_funct7;
  logic[4:0] alu_ctrl_alu_function;

endmodule

`endif // SINGLECYCLE_CTLPATH_H
