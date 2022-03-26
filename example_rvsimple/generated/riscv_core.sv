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
  output logic[31:0] bus_address,
  output logic[31:0] bus_write_data2,
  output logic[3:0]  bus_byte_enable2,
  output logic  bus_read_enable2,
  output logic  bus_write_enable2,
  output logic[31:0] bus_address2,
  output logic[31:0] pc
);
 /*public:*/
  /*logic<32> bus_address;*/

  always_comb begin bus_write_data2 = dmem_bus_write_data; end
  always_comb begin bus_byte_enable2 = dmem_bus_byte_enable; end
  always_comb begin bus_read_enable2 = ctlpath_data_mem_read_enable; end
  always_comb begin bus_write_enable2 = ctlpath_data_mem_write_enable; end
  always_comb begin bus_address2 = datapath_data_mem_address; end
  always_comb begin pc = datapath_pc2; end

  //----------------------------------------

  initial begin : init
    /*datapath.init();*/
  end

  always_comb begin : tock_pc
    /*datapath.tock_program_counter(
      reset,
      ctlpath.pc_write_enable2()
    );*/
    datapath_reset = reset;
    datapath_pc_write_enable = ctlpath_pc_write_enable2;
  end

  always_comb begin : tock_regs
    /*datapath.tock_regs(
      inst,
      ctlpath.regfile_write_enable2()
    );*/
    datapath_inst = inst;
    datapath_regfile_write_enable = ctlpath_regfile_write_enable2;
  end

  always_comb begin : tock_execute
    /*datapath.tock_decode(inst);*/
    datapath_inst = inst;
    /*datapath.tock_regfile(inst);*/
    datapath_inst = inst;

    /*ctlpath.tock_decode(
      datapath.inst_opcode2(inst)
    );*/
    ctlpath_inst_opcode = datapath_inst_opcode2;

    /*ctlpath.tock_alu_control(
      datapath.inst_funct32(inst),
      datapath.inst_funct72(inst)
    );*/
    ctlpath_inst_funct3 = datapath_inst_funct32;
    ctlpath_inst_funct7 = datapath_inst_funct72;

    /*datapath.tock_alu(
      ctlpath.alu_function(),
      ctlpath.alu_operand_a_select(),
      ctlpath.alu_operand_b_select()
    );*/
    datapath_alu_function = ctlpath_alu_function;
    datapath_alu_operand_a_select = ctlpath_alu_operand_a_select;
    datapath_alu_operand_b_select = ctlpath_alu_operand_b_select;

    /*ctlpath.tock_next_pc_select(
      datapath.inst_opcode2(inst),
      datapath.inst_funct32(inst),
      datapath.alu_result_equal_zero2()
    );*/
    ctlpath_inst_opcode = datapath_inst_opcode2;
    ctlpath_inst_funct3 = datapath_inst_funct32;
    ctlpath_alu_result_equal_zero = datapath_alu_result_equal_zero2;

    /*datapath.tock_next_pc(ctlpath.next_pc_select());*/
    datapath_next_pc_select = ctlpath_next_pc_select;

    /*dmem.tock_bus(
      datapath.inst_funct32(inst),
      datapath.data_mem_address,
      datapath.data_mem_write_data2());*/
    dmem_data_format = datapath_inst_funct32;
    dmem_address = datapath_data_mem_address;
    dmem_write_data = datapath_data_mem_write_data2;

    bus_address = datapath_data_mem_address;
  end

  always_comb begin : tock_writeback
    /*dmem.tock_position_fix(datapath.data_mem_address, bus_read_data);*/
    dmem_address = datapath_data_mem_address;
    dmem_bus_read_data = bus_read_data;
    /*dmem.tock_sign_fix(datapath.inst_funct32(inst));*/
    dmem_data_format = datapath_inst_funct32;
    /*dmem.tock_read_data();*/
    /*datapath.tock_writeback(dmem.read_data, ctlpath.reg_writeback_select(), inst);*/
    datapath_data_mem_read_data = dmem_read_data;
    datapath_reg_writeback_select = ctlpath_reg_writeback_select;
    datapath_inst = inst;
  end

  //----------------------------------------

 /*private:*/
  singlecycle_datapath datapath(
    // Inputs
    .clock(clock),
    .inst(datapath_inst), 
    .reset(datapath_reset), 
    .pc_write_enable(datapath_pc_write_enable), 
    .regfile_write_enable(datapath_regfile_write_enable), 
    .alu_function(datapath_alu_function), 
    .alu_operand_a_select(datapath_alu_operand_a_select), 
    .alu_operand_b_select(datapath_alu_operand_b_select), 
    .next_pc_select(datapath_next_pc_select), 
    .data_mem_read_data(datapath_data_mem_read_data), 
    .reg_writeback_select(datapath_reg_writeback_select), 
    // Outputs
    .data_mem_address(datapath_data_mem_address), 
    .pc2(datapath_pc2), 
    .data_mem_address2(datapath_data_mem_address2), 
    .data_mem_write_data2(datapath_data_mem_write_data2), 
    .alu_result_equal_zero2(datapath_alu_result_equal_zero2), 
    .inst_opcode2(datapath_inst_opcode2), 
    .inst_funct32(datapath_inst_funct32), 
    .inst_funct72(datapath_inst_funct72)
  );
  logic[31:0] datapath_inst;
  logic datapath_reset;
  logic datapath_pc_write_enable;
  logic datapath_regfile_write_enable;
  logic[4:0] datapath_alu_function;
  logic datapath_alu_operand_a_select;
  logic datapath_alu_operand_b_select;
  logic[1:0] datapath_next_pc_select;
  logic[31:0] datapath_data_mem_read_data;
  logic[2:0] datapath_reg_writeback_select;
  logic[31:0] datapath_data_mem_address;
  logic[31:0] datapath_pc2;
  logic[31:0] datapath_data_mem_address2;
  logic[31:0] datapath_data_mem_write_data2;
  logic  datapath_alu_result_equal_zero2;
  logic[6:0]  datapath_inst_opcode2;
  logic[2:0]  datapath_inst_funct32;
  logic[6:0]  datapath_inst_funct72;

  singlecycle_ctlpath ctlpath(
    // Inputs
    .clock(clock),
    .inst_opcode(ctlpath_inst_opcode), 
    .inst_funct3(ctlpath_inst_funct3), 
    .alu_result_equal_zero(ctlpath_alu_result_equal_zero), 
    .inst_funct7(ctlpath_inst_funct7), 
    // Outputs
    .alu_function(ctlpath_alu_function), 
    .pc_write_enable2(ctlpath_pc_write_enable2), 
    .regfile_write_enable2(ctlpath_regfile_write_enable2), 
    .alu_operand_a_select(ctlpath_alu_operand_a_select), 
    .alu_operand_b_select(ctlpath_alu_operand_b_select), 
    .data_mem_read_enable(ctlpath_data_mem_read_enable), 
    .data_mem_write_enable(ctlpath_data_mem_write_enable), 
    .reg_writeback_select(ctlpath_reg_writeback_select), 
    .next_pc_select(ctlpath_next_pc_select)
  );
  logic[6:0] ctlpath_inst_opcode;
  logic[2:0] ctlpath_inst_funct3;
  logic ctlpath_alu_result_equal_zero;
  logic[6:0] ctlpath_inst_funct7;
  logic[4:0] ctlpath_alu_function;
  logic ctlpath_pc_write_enable2;
  logic ctlpath_regfile_write_enable2;
  logic ctlpath_alu_operand_a_select;
  logic ctlpath_alu_operand_b_select;
  logic ctlpath_data_mem_read_enable;
  logic ctlpath_data_mem_write_enable;
  logic[2:0] ctlpath_reg_writeback_select;
  logic[1:0] ctlpath_next_pc_select;

  data_memory_interface dmem(
    // Inputs
    .clock(clock),
    .data_format(dmem_data_format), 
    .address(dmem_address), 
    .write_data(dmem_write_data), 
    .bus_read_data(dmem_bus_read_data), 
    // Outputs
    .read_data(dmem_read_data), 
    .bus_write_data(dmem_bus_write_data), 
    .bus_byte_enable(dmem_bus_byte_enable)
  );
  logic[2:0] dmem_data_format;
  logic[31:0] dmem_address;
  logic[31:0] dmem_write_data;
  logic[31:0] dmem_bus_read_data;
  logic[31:0] dmem_read_data;
  logic[31:0] dmem_bus_write_data;
  logic[3:0] dmem_bus_byte_enable;

endmodule;

`endif  // RVSIMPLE_RISCV_CORE_H

