// RISC-V SiMPLE SV -- Single-cycle RISC-V core
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

`ifndef RVSIMPLE_RISCV_CORE_H
`define RVSIMPLE_RISCV_CORE_H

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
   input logic[31:0] inst,
   input logic[31:0] bus_read_data,
   input logic[31:0] alu_result2,
   output logic[31:0] bus_write_data2,
   output logic[3:0]  bus_byte_enable2,
   output logic  bus_read_enable2,
   output logic  bus_write_enable2,
   output logic[31:0] pc,
   output logic[31:0] alu_result
);
 /*public:*/

   always_comb begin
    dmem_address = alu_result2;
    dmem_write_data = datapath_rs2_data;
    datapath_inst = inst;
    bus_write_data2 = dmem_bus_write_data;
  end

  always_comb begin
    logic[2:0] funct3;
    datapath_inst = inst;
    funct3 = datapath_inst_funct3;
    dmem_data_format = funct3;
    dmem_address = alu_result2;
    bus_byte_enable2 = dmem_bus_byte_enable;
  end

  always_comb begin
    logic[6:0] opcode;
    datapath_inst = inst;
    opcode = datapath_inst_opcode;
    ctlpath_inst_opcode = opcode;
    bus_read_enable2 = ctlpath_data_mem_read_enable;
  end

  always_comb begin
    logic[6:0] opcode;
    datapath_inst = inst;
    opcode = datapath_inst_opcode;
    ctlpath_inst_opcode = opcode;
    bus_write_enable2 = ctlpath_data_mem_write_enable;
  end

  always_comb begin
    pc = datapath_pc;
  end

  //----------------------------------------

  always_comb begin
    logic[6:0] opcode;
    logic[2:0] funct3;
    logic[6:0] funct7;
    logic[4:0] alu_function;
    datapath_inst = inst;
    opcode = datapath_inst_opcode;
    datapath_inst = inst;
    funct3 = datapath_inst_funct3;
    datapath_inst = inst;
    funct7 = datapath_inst_funct7;

    ctlpath_inst_opcode = opcode;
    ctlpath_inst_funct3 = funct3;
    ctlpath_inst_funct7 = funct7;
    alu_function = ctlpath_alu_function;

    datapath_inst = inst;
    datapath_alu_function = alu_function;
    datapath_alu_operand_a_select = ctlpath_alu_operand_a_select;
    datapath_alu_operand_b_select = ctlpath_alu_operand_b_select;
    ctlpath_inst_opcode = opcode;
    ctlpath_inst_opcode = opcode;
    alu_result = datapath_alu_result;
  end

  always_comb begin : tock
    logic[6:0] opcode;
    logic[2:0] funct3;
    logic reg_we;
    logic[31:0] mem_data;
    logic[2:0] reg_select;
    logic[1:0] pc_select;
    logic pc_we;
    datapath_inst = inst;
    opcode = datapath_inst_opcode;
    datapath_inst = inst;
    funct3 = datapath_inst_funct3;
    ctlpath_inst_opcode = opcode;
    reg_we = ctlpath_regfile_write_enable;

    dmem_address = alu_result2;
    dmem_bus_read_data = bus_read_data;
    dmem_data_format = funct3;
    mem_data = dmem_read_data;
    ctlpath_inst_opcode = opcode;
    reg_select = ctlpath_reg_writeback_select;
    ctlpath_inst_opcode = opcode;
    ctlpath_inst_funct3 = funct3;
    ctlpath_alu_result_equal_zero = alu_result2 == 0;
    pc_select = ctlpath_next_pc_select;
    ctlpath_inst_opcode = opcode;
    pc_we = ctlpath_pc_write_enable;

    datapath_reset = reset;
    datapath_inst = inst;
    datapath_regfile_write_enable = reg_we;
    datapath_data_mem_read_data = mem_data;
    datapath_reg_writeback_select = reg_select;
    datapath_alu_result2 = alu_result2;
    datapath_next_pc_select = pc_select;
    datapath_pc_write_enable = pc_we;
    /*datapath.tock(
      reset,
      inst,
      reg_we,
      mem_data,
      reg_select,
      alu_result2,
      pc_select,
      pc_we
    );*/
  end


  //----------------------------------------

 /*private:*/
  singlecycle_datapath datapath(
     // Inputs
     .clock(clock),
     .reset(datapath_reset), 
     .inst(datapath_inst), 
     .regfile_write_enable(datapath_regfile_write_enable), 
     .data_mem_read_data(datapath_data_mem_read_data), 
     .reg_writeback_select(datapath_reg_writeback_select), 
     .alu_result2(datapath_alu_result2), 
     .next_pc_select(datapath_next_pc_select), 
     .pc_write_enable(datapath_pc_write_enable), 
     .alu_function(datapath_alu_function), 
     .alu_operand_a_select(datapath_alu_operand_a_select), 
     .alu_operand_b_select(datapath_alu_operand_b_select), 
     // Outputs
     .pc(datapath_pc), 
     .rs2_data(datapath_rs2_data), 
     .inst_opcode(datapath_inst_opcode), 
     .inst_funct3(datapath_inst_funct3), 
     .inst_funct7(datapath_inst_funct7), 
     .alu_result(datapath_alu_result)
   );
   logic datapath_reset;
   logic[31:0] datapath_inst;
   logic datapath_regfile_write_enable;
   logic[31:0] datapath_data_mem_read_data;
   logic[2:0] datapath_reg_writeback_select;
   logic[31:0] datapath_alu_result2;
   logic[1:0] datapath_next_pc_select;
   logic datapath_pc_write_enable;
   logic[4:0] datapath_alu_function;
   logic datapath_alu_operand_a_select;
   logic datapath_alu_operand_b_select;
   logic[31:0] datapath_pc;
   logic[31:0] datapath_rs2_data;
   logic[6:0]  datapath_inst_opcode;
   logic[2:0]  datapath_inst_funct3;
   logic[6:0]  datapath_inst_funct7;
   logic[31:0] datapath_alu_result;

  singlecycle_ctlpath ctlpath(
     // Inputs
     .clock(clock),
     .inst_opcode(ctlpath_inst_opcode), 
     .inst_funct3(ctlpath_inst_funct3), 
     .inst_funct7(ctlpath_inst_funct7), 
     .alu_result_equal_zero(ctlpath_alu_result_equal_zero), 
     // Outputs
     .alu_function(ctlpath_alu_function), 
     .pc_write_enable(ctlpath_pc_write_enable), 
     .regfile_write_enable(ctlpath_regfile_write_enable), 
     .alu_operand_a_select(ctlpath_alu_operand_a_select), 
     .alu_operand_b_select(ctlpath_alu_operand_b_select), 
     .data_mem_read_enable(ctlpath_data_mem_read_enable), 
     .data_mem_write_enable(ctlpath_data_mem_write_enable), 
     .reg_writeback_select(ctlpath_reg_writeback_select), 
     .next_pc_select(ctlpath_next_pc_select)
   );
   logic[6:0] ctlpath_inst_opcode;
   logic[2:0] ctlpath_inst_funct3;
   logic[6:0] ctlpath_inst_funct7;
   logic ctlpath_alu_result_equal_zero;
   logic[4:0] ctlpath_alu_function;
   logic ctlpath_pc_write_enable;
   logic ctlpath_regfile_write_enable;
   logic ctlpath_alu_operand_a_select;
   logic ctlpath_alu_operand_b_select;
   logic ctlpath_data_mem_read_enable;
   logic ctlpath_data_mem_write_enable;
   logic[2:0] ctlpath_reg_writeback_select;
   logic[1:0] ctlpath_next_pc_select;

  data_memory_interface dmem(
     // Inputs
     .clock(clock),
     .address(dmem_address), 
     .write_data(dmem_write_data), 
     .data_format(dmem_data_format), 
     .bus_read_data(dmem_bus_read_data), 
     // Outputs
     .bus_write_data(dmem_bus_write_data), 
     .bus_byte_enable(dmem_bus_byte_enable), 
     .read_data(dmem_read_data)
   );
   logic[31:0] dmem_address;
   logic[31:0] dmem_write_data;
   logic[2:0] dmem_data_format;
   logic[31:0] dmem_bus_read_data;
   logic[31:0] dmem_bus_write_data;
   logic[3:0] dmem_bus_byte_enable;
   logic[31:0] dmem_read_data;

endmodule;

`endif  // RVSIMPLE_RISCV_CORE_H

