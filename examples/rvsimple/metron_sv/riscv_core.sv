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
   input logic[31:0] inst,
   input logic reset,
   input logic[31:0] bus_read_data,
   output logic[31:0] alu_result,
   output logic[31:0] bus_write_data,
   output logic[31:0] pc,
   output logic bus_write_enable2,
   output logic[3:0] bus_byte_enable2,
   output logic bus_read_enable2
);
 /*public:*/

   /*logic<32> inst;*/
   /*logic<32> alu_result;*/
   /*logic<1> reset;*/
   /*logic<32> bus_read_data;*/
   /*logic<32> bus_write_data;*/

  /*logic<32> pc;*/
  always_comb begin /*tock_pc*/ pc = datapath_pc; end

  always_comb begin /*tock_inst*/
    datapath_inst = inst;
    /*datapath.tock_inst()*/;
  end

  always_comb begin /*tock_alu_result*/
    logic[6:0] opcode;
    logic[2:0] funct3;
    logic[6:0] funct7;
    opcode = datapath_inst_opcode;
    funct3 = datapath_inst_funct3;
    funct7 = datapath_inst_funct7;

    ctlpath_inst_opcode = opcode;
    ctlpath_inst_funct3 = funct3;
    ctlpath_inst_funct7 = funct7;
    /*ctlpath.tock_alu_function()*/;
    /*ctlpath.tock_alu_operand_a_select()*/;
    /*ctlpath.tock_alu_operand_b_select()*/;

    datapath_alu_function = ctlpath_alu_function;
    datapath_alu_operand_a_select = ctlpath_alu_operand_a_select;
    datapath_alu_operand_b_select = ctlpath_alu_operand_b_select;
    /*datapath.tock_alu_result()*/;
    alu_result = datapath_alu_result;

    dmem_address = alu_result;
    dmem_data_format = funct3;
    /*dmem.tock_inputs()*/;
  end

  always_comb begin /*tock*/
    ctlpath_alu_result_equal_zero = alu_result == 0;
    /*ctlpath.tock_regfile_write_enable()*/;
    /*ctlpath.tock_reg_writeback_select()*/;
    /*ctlpath.tock_next_pc_select()*/;
    /*ctlpath.tock_pc_write_enable()*/;

    dmem_bus_read_data = bus_read_data;
    /*dmem.tock_read_data()*/;

    datapath_reset = reset;
    datapath_regfile_write_enable = ctlpath_regfile_write_enable;
    datapath_data_mem_read_data = dmem_read_data;
    datapath_reg_writeback_select = ctlpath_reg_writeback_select;
    datapath_next_pc_select = ctlpath_next_pc_select;
    datapath_pc_write_enable = ctlpath_pc_write_enable;
    /*datapath.tock()*/;
  end


  always_comb begin /*tock_bus_write_data2*/
    dmem_write_data = datapath_temp_rs2_data;
    bus_write_data = dmem_bus_write_data;
  end

  /*logic<1> bus_write_enable2;*/
  always_comb begin /*tock_bus_write_enable2*/
    bus_write_enable2 = ctlpath_data_mem_write_enable;
  end

  /*logic<4> bus_byte_enable2;*/
  always_comb begin /*tock_bus_byte_enable2*/
    logic[2:0] funct3;
    funct3 = datapath_inst_funct3;
    bus_byte_enable2 = dmem_bus_byte_enable;
  end

  /*logic<1> bus_read_enable2;*/
    always_comb begin /*tock_bus_read_enable2*/
      bus_read_enable2 = ctlpath_data_mem_read_enable;
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
     .next_pc_select(datapath_next_pc_select), 
     .pc_write_enable(datapath_pc_write_enable), 
     .alu_function(datapath_alu_function), 
     .alu_operand_a_select(datapath_alu_operand_a_select), 
     .alu_operand_b_select(datapath_alu_operand_b_select), 
     // Outputs
     .inst_rd(datapath_inst_rd), 
     .inst_rs1(datapath_inst_rs1), 
     .inst_rs2(datapath_inst_rs2), 
     .inst_opcode(datapath_inst_opcode), 
     .inst_funct3(datapath_inst_funct3), 
     .inst_funct7(datapath_inst_funct7), 
     .inst_immediate(datapath_inst_immediate), 
     .temp_rs1_data(datapath_temp_rs1_data), 
     .temp_rs2_data(datapath_temp_rs2_data), 
     .alu_result(datapath_alu_result), 
     .pc(datapath_pc)
   );
   logic datapath_reset;
   logic[31:0] datapath_inst;
   logic datapath_regfile_write_enable;
   logic[31:0] datapath_data_mem_read_data;
   logic[2:0] datapath_reg_writeback_select;
   logic[1:0] datapath_next_pc_select;
   logic datapath_pc_write_enable;
   logic[4:0] datapath_alu_function;
   logic datapath_alu_operand_a_select;
   logic datapath_alu_operand_b_select;
   logic[4:0] datapath_inst_rd;
   logic[4:0] datapath_inst_rs1;
   logic[4:0] datapath_inst_rs2;
   logic[6:0] datapath_inst_opcode;
   logic[2:0] datapath_inst_funct3;
   logic[6:0] datapath_inst_funct7;
   logic[31:0] datapath_inst_immediate;
   logic[31:0] datapath_temp_rs1_data;
   logic[31:0] datapath_temp_rs2_data;
   logic[31:0] datapath_alu_result;
   logic[31:0] datapath_pc;

  singlecycle_ctlpath ctlpath(
     // Inputs
     .clock(clock),
     .inst_opcode(ctlpath_inst_opcode), 
     .inst_funct3(ctlpath_inst_funct3), 
     .alu_result_equal_zero(ctlpath_alu_result_equal_zero), 
     .inst_funct7(ctlpath_inst_funct7), 
     // Outputs
     .alu_function(ctlpath_alu_function), 
     .pc_write_enable(ctlpath_pc_write_enable), 
     .regfile_write_enable(ctlpath_regfile_write_enable), 
     .alu_operand_a_select(ctlpath_alu_operand_a_select), 
     .alu_operand_b_select(ctlpath_alu_operand_b_select), 
     .reg_writeback_select(ctlpath_reg_writeback_select), 
     .next_pc_select(ctlpath_next_pc_select), 
     .data_mem_read_enable(ctlpath_data_mem_read_enable), 
     .data_mem_write_enable(ctlpath_data_mem_write_enable)
   );
   logic[6:0] ctlpath_inst_opcode;
   logic[2:0] ctlpath_inst_funct3;
   logic ctlpath_alu_result_equal_zero;
   logic[6:0] ctlpath_inst_funct7;
   logic[4:0] ctlpath_alu_function;
   logic ctlpath_pc_write_enable;
   logic ctlpath_regfile_write_enable;
   logic ctlpath_alu_operand_a_select;
   logic ctlpath_alu_operand_b_select;
   logic[2:0] ctlpath_reg_writeback_select;
   logic[1:0] ctlpath_next_pc_select;
   logic ctlpath_data_mem_read_enable;
   logic ctlpath_data_mem_write_enable;

  data_memory_interface dmem(
     // Inputs
     .clock(clock),
     .bus_read_data(dmem_bus_read_data), 
     .address(dmem_address), 
     .data_format(dmem_data_format), 
     .write_data(dmem_write_data), 
     // Outputs
     .address2(dmem_address2), 
     .data_format2(dmem_data_format2), 
     .read_data(dmem_read_data), 
     .bus_write_data(dmem_bus_write_data), 
     .bus_byte_enable(dmem_bus_byte_enable)
   );
   logic[31:0] dmem_bus_read_data;
   logic[31:0] dmem_address;
   logic[2:0] dmem_data_format;
   logic[31:0] dmem_write_data;
   logic[31:0] dmem_address2;
   logic[2:0] dmem_data_format2;
   logic[31:0] dmem_read_data;
   logic[31:0] dmem_bus_write_data;
   logic[3:0] dmem_bus_byte_enable;

endmodule;

`endif  // RVSIMPLE_RISCV_CORE_H

