// RISC-V SiMPLE SV -- control path
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

`ifndef RVSIMPLE_SINGLECYCLE_CTLPATH_H
`define RVSIMPLE_SINGLECYCLE_CTLPATH_H

`include "alu_control.sv"
`include "config.sv"
`include "constants.sv"
`include "control_transfer.sv"
`include "metron_tools.sv"
`include "singlecycle_control.sv"

module singlecycle_ctlpath
(
  input logic clock,
  input logic[6:0] inst_opcode,
  input logic[2:0] inst_funct3,
  input logic[6:0] inst_funct7,
  input logic alu_result_equal_zero,
  output logic[4:0] alu_function,
  output logic pc_write_enable,
  output logic regfile_write_enable,
  output logic alu_operand_a_select,
  output logic alu_operand_b_select,
  output logic data_mem_read_enable,
  output logic data_mem_write_enable,
  output logic[2:0] reg_writeback_select,
  output logic[1:0] next_pc_select
);
 /*public:*/

  always_comb begin
    alu_ctrl_alu_op_type = control_alu_op_type2;
    alu_ctrl_inst_funct3 = inst_funct3;
    alu_ctrl_inst_funct7 = inst_funct7;
    control_inst_opcode = inst_opcode;
    alu_function = alu_ctrl_alu_function;
  end

  always_comb begin control_inst_opcode = inst_opcode;
pc_write_enable = control_pc_write_enable; end
  always_comb begin control_inst_opcode = inst_opcode;
regfile_write_enable = control_regfile_write_enable; end
  always_comb begin control_inst_opcode = inst_opcode;
alu_operand_a_select = control_alu_operand_a_select; end
  always_comb begin control_inst_opcode = inst_opcode;
alu_operand_b_select = control_alu_operand_b_select; end
  always_comb begin control_inst_opcode = inst_opcode;
data_mem_read_enable = control_data_mem_read_enable; end
  always_comb begin control_inst_opcode = inst_opcode;
data_mem_write_enable = control_data_mem_write_enable; end
  always_comb begin control_inst_opcode = inst_opcode;
reg_writeback_select = control_reg_writeback_select; end

  always_comb begin
    logic take_branch;
    transfer_result_equal_zero = alu_result_equal_zero;
    transfer_inst_funct3 = inst_funct3;
    take_branch = transfer_take_branch;
    control_inst_opcode = inst_opcode;
    control_take_branch = take_branch;
    next_pc_select = control_next_pc_select;
  end

  //----------------------------------------

 /*private:*/
  singlecycle_control control(
    // Inputs
    .clock(clock),
    .inst_opcode(control_inst_opcode), 
    .take_branch(control_take_branch), 
    // Outputs
    .next_pc_select(control_next_pc_select), 
    .pc_write_enable(control_pc_write_enable), 
    .regfile_write_enable(control_regfile_write_enable), 
    .alu_operand_a_select(control_alu_operand_a_select), 
    .alu_operand_b_select(control_alu_operand_b_select), 
    .alu_op_type2(control_alu_op_type2), 
    .data_mem_read_enable(control_data_mem_read_enable), 
    .data_mem_write_enable(control_data_mem_write_enable), 
    .reg_writeback_select(control_reg_writeback_select)
  );
  logic[6:0] control_inst_opcode;
  logic control_take_branch;
  logic[1:0] control_next_pc_select;
  logic control_pc_write_enable;
  logic control_regfile_write_enable;
  logic control_alu_operand_a_select;
  logic control_alu_operand_b_select;
  logic[1:0] control_alu_op_type2;
  logic control_data_mem_read_enable;
  logic control_data_mem_write_enable;
  logic[2:0] control_reg_writeback_select;

  control_transfer transfer(
    // Inputs
    .clock(clock),
    .result_equal_zero(transfer_result_equal_zero), 
    .inst_funct3(transfer_inst_funct3), 
    // Outputs
    .take_branch(transfer_take_branch)
  );
  logic transfer_result_equal_zero;
  logic[2:0] transfer_inst_funct3;
  logic transfer_take_branch;

  alu_control alu_ctrl(
    // Inputs
    .clock(clock),
    .alu_op_type(alu_ctrl_alu_op_type), 
    .inst_funct3(alu_ctrl_inst_funct3), 
    .inst_funct7(alu_ctrl_inst_funct7), 
    // Outputs
    .alu_function(alu_ctrl_alu_function)
  );
  logic[1:0] alu_ctrl_alu_op_type;
  logic[2:0] alu_ctrl_inst_funct3;
  logic[6:0] alu_ctrl_inst_funct7;
  logic[4:0] alu_ctrl_alu_function;

endmodule;

`endif  // RVSIMPLE_SINGLECYCLE_CTLPATH_H

