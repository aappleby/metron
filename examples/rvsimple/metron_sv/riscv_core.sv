// RISC-V SiMPLE SV -- Single-cycle RISC-V core
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

`ifndef RISCV_CORE_H
`define RISCV_CORE_H

`include "config.sv"
`include "constants.sv"
`include "data_memory_interface.sv"
`include "metron_tools.sv"
`include "singlecycle_ctlpath.sv"
`include "singlecycle_datapath.sv"

module riscv_core
(
  input logic clock,
  input logic reset,
  output logic[31:0] bus_address,
  input logic[31:0] bus_read_data,
  output logic[31:0] bus_write_data,
  output logic[3:0] bus_byte_enable,
  output logic bus_read_enable,
  output logic bus_write_enable,
  input logic[31:0] inst,
  output logic[31:0] pc
);
 /*public:*/


  function tock_pc();
    pc = datapath_pc;
  endfunction
  always_comb tock_pc();

  function tock_execute();
    datapath_inst = inst;

    ctlpath_inst_opcode = datapath_inst_opcode;
    ctlpath_inst_funct3 = datapath_inst_funct3;
    ctlpath_inst_funct7 = datapath_inst_funct7;


    datapath_alu_function = ctlpath_alu_function;
    datapath_alu_operand_a_select = ctlpath_alu_operand_a_select;
    datapath_alu_operand_b_select = ctlpath_alu_operand_b_select;


    dmem_read_enable = ctlpath_data_mem_read_enable;
    dmem_write_enable = ctlpath_data_mem_write_enable;
    dmem_data_format = datapath_inst_funct3;
    dmem_address = datapath_data_mem_address;
    dmem_write_data = datapath_data_mem_write_data;

    ctlpath_alu_result_equal_zero = datapath_alu_result_equal_zero;

    //----------

    bus_address = dmem_bus_address;
    bus_write_data = dmem_bus_write_data;
    bus_byte_enable = dmem_bus_byte_enable;
    bus_read_enable = dmem_bus_read_enable;
    bus_write_enable = dmem_bus_write_enable;
  endfunction
  always_comb tock_execute();

  function tock_writeback();
    dmem_bus_read_data = bus_read_data;

    datapath_next_pc_select = ctlpath_next_pc_select;

    datapath_reset = reset;
    datapath_pc_write_enable = ctlpath_pc_write_enable;

    datapath_reg_writeback_select = ctlpath_reg_writeback_select;
    datapath_data_mem_read_data = dmem_read_data;

    datapath_regfile_write_enable = ctlpath_regfile_write_enable;
  endfunction
  always_comb tock_writeback();

  //----------------------------------------

 /*private:*/
  singlecycle_datapath datapath(
    .clock(clock),
    .reset(datapath_reset),
    .data_mem_read_data(datapath_data_mem_read_data),
    .data_mem_address(datapath_data_mem_address),
    .data_mem_write_data(datapath_data_mem_write_data),
    .inst(datapath_inst),
    .pc(datapath_pc),
    .inst_opcode(datapath_inst_opcode),
    .inst_funct3(datapath_inst_funct3),
    .inst_funct7(datapath_inst_funct7),
    .alu_result_equal_zero(datapath_alu_result_equal_zero),
    .pc_write_enable(datapath_pc_write_enable),
    .regfile_write_enable(datapath_regfile_write_enable),
    .alu_operand_a_select(datapath_alu_operand_a_select),
    .alu_operand_b_select(datapath_alu_operand_b_select),
    .reg_writeback_select(datapath_reg_writeback_select),
    .next_pc_select(datapath_next_pc_select),
    .alu_function(datapath_alu_function)
  );
  logic datapath_reset;
  logic[31:0] datapath_data_mem_read_data;
  logic[31:0] datapath_inst;
  logic datapath_pc_write_enable;
  logic datapath_regfile_write_enable;
  logic datapath_alu_operand_a_select;
  logic datapath_alu_operand_b_select;
  logic[2:0] datapath_reg_writeback_select;
  logic[1:0] datapath_next_pc_select;
  logic[4:0] datapath_alu_function;
  logic[31:0] datapath_data_mem_address;
  logic[31:0] datapath_data_mem_write_data;
  logic[31:0] datapath_pc;
  logic[6:0] datapath_inst_opcode;
  logic[2:0] datapath_inst_funct3;
  logic[6:0] datapath_inst_funct7;
  logic datapath_alu_result_equal_zero;

  singlecycle_ctlpath ctlpath(
    .inst_opcode(ctlpath_inst_opcode),
    .inst_funct3(ctlpath_inst_funct3),
    .inst_funct7(ctlpath_inst_funct7),
    .alu_result_equal_zero(ctlpath_alu_result_equal_zero),
    .pc_write_enable(ctlpath_pc_write_enable),
    .regfile_write_enable(ctlpath_regfile_write_enable),
    .alu_operand_a_select(ctlpath_alu_operand_a_select),
    .alu_operand_b_select(ctlpath_alu_operand_b_select),
    .data_mem_read_enable(ctlpath_data_mem_read_enable),
    .data_mem_write_enable(ctlpath_data_mem_write_enable),
    .reg_writeback_select(ctlpath_reg_writeback_select),
    .alu_function(ctlpath_alu_function),
    .next_pc_select(ctlpath_next_pc_select)
  );
  logic[6:0] ctlpath_inst_opcode;
  logic[2:0] ctlpath_inst_funct3;
  logic[6:0] ctlpath_inst_funct7;
  logic ctlpath_alu_result_equal_zero;
  logic ctlpath_pc_write_enable;
  logic ctlpath_regfile_write_enable;
  logic ctlpath_alu_operand_a_select;
  logic ctlpath_alu_operand_b_select;
  logic ctlpath_data_mem_read_enable;
  logic ctlpath_data_mem_write_enable;
  logic[2:0] ctlpath_reg_writeback_select;
  logic[4:0] ctlpath_alu_function;
  logic[1:0] ctlpath_next_pc_select;

  data_memory_interface dmem(
    .read_enable(dmem_read_enable),
    .write_enable(dmem_write_enable),
    .data_format(dmem_data_format),
    .address(dmem_address),
    .write_data(dmem_write_data),
    .read_data(dmem_read_data),
    .bus_address(dmem_bus_address),
    .bus_read_data(dmem_bus_read_data),
    .bus_write_data(dmem_bus_write_data),
    .bus_byte_enable(dmem_bus_byte_enable),
    .bus_read_enable(dmem_bus_read_enable),
    .bus_write_enable(dmem_bus_write_enable)
  );
  logic dmem_read_enable;
  logic dmem_write_enable;
  logic[2:0] dmem_data_format;
  logic[31:0] dmem_address;
  logic[31:0] dmem_write_data;
  logic[31:0] dmem_bus_read_data;
  logic[31:0] dmem_read_data;
  logic[31:0] dmem_bus_address;
  logic[31:0] dmem_bus_write_data;
  logic[3:0] dmem_bus_byte_enable;
  logic dmem_bus_read_enable;
  logic dmem_bus_write_enable;

endmodule

`endif // RISCV_CORE_H
