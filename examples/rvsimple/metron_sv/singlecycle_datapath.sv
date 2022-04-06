// RISC-V SiMPLE SV -- single-cycle data path
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

`ifndef RVSIMPLE_SINGLECYCLE_DATAPATH_H
`define RVSIMPLE_SINGLECYCLE_DATAPATH_H

`include "adder.sv"
`include "alu.sv"
`include "config.sv"
`include "constants.sv"
`include "immediate_generator.sv"
`include "instruction_decoder.sv"
`include "metron_tools.sv"
`include "multiplexer2.sv"
`include "multiplexer4.sv"
`include "multiplexer8.sv"
`include "regfile.sv"
`include "register.sv"

import rv_config::*;

module singlecycle_datapath
(
  input logic clock,
  input logic[31:0] inst,
  input logic[4:0] alu_function,
  input logic alu_operand_a_select,
  input logic alu_operand_b_select,
  input logic reset,
  input logic regfile_write_enable,
  input logic[31:0] data_mem_read_data,
  input logic[2:0] reg_writeback_select,
  input logic[31:0] alu_result2,
  input logic[1:0] next_pc_select,
  input logic pc_write_enable,
  output logic[31:0] pc,
  output logic[31:0] rs2_data,
  output logic[6:0]  inst_opcode,
  output logic[2:0]  inst_funct3,
  output logic[6:0]  inst_funct7,
  output logic[31:0] alu_result
);
 /*public:*/

  //----------------------------------------

  always_comb begin
    pc = program_counter_value;
  end

  always_comb begin
    logic[4:0] rs2_idx;
    idec_inst = inst;
    rs2_idx = idec_inst_rs2;
    regs_rs2_address = rs2_idx;
    rs2_data = regs_rs2_data;
  end

  always_comb begin idec_inst = inst;
inst_opcode = idec_inst_opcode; end
  always_comb begin idec_inst = inst;
inst_funct3 = idec_inst_funct3; end
  always_comb begin idec_inst = inst;
inst_funct7 = idec_inst_funct7; end

  //----------------------------------------

  always_comb begin
    logic[4:0] rs1_idx;
    logic[4:0] rs2_idx;
    idec_inst = inst;
    rs1_idx = idec_inst_rs1;
    idec_inst = inst;
    rs2_idx = idec_inst_rs2;

    regs_rs1_address = rs1_idx;
    mux_operand_a_sel = alu_operand_a_select;
    mux_operand_a_in0 = regs_rs1_data;
    mux_operand_a_in1 = program_counter_value;
    regs_rs2_address = rs2_idx;
    igen_inst = inst;
    mux_operand_b_sel = alu_operand_b_select;
    mux_operand_b_in0 = regs_rs2_data;
    mux_operand_b_in1 = igen_immediate;
    alu_core_alu_function = alu_function;
    alu_core_operand_a = mux_operand_a_out;
    alu_core_operand_b = mux_operand_b_out;
    alu_result = alu_core_alu_result;
  end

  always_comb begin /*tock*/
    logic[31:0] pc_plus_4;
    logic[31:0] pc_plus_imm;
    logic[31:0] pc_data;
    logic[31:0] reg_data;
    adder_pc_plus_4_operand_a = 32'h00000004;
    adder_pc_plus_4_operand_b = program_counter_value;
    pc_plus_4 = adder_pc_plus_4_adder_result;
    igen_inst = inst;
    adder_pc_plus_immediate_operand_a = program_counter_value;
    adder_pc_plus_immediate_operand_b = igen_immediate;
    pc_plus_imm = adder_pc_plus_immediate_adder_result;

    mux_next_pc_select_sel = next_pc_select;
    mux_next_pc_select_in0 = pc_plus_4;
    mux_next_pc_select_in1 = pc_plus_imm;
    mux_next_pc_select_in2 = {alu_result2[31:1], 1'b0};
    mux_next_pc_select_in3 = 32'b0;
    pc_data = mux_next_pc_select_out;
    program_counter_reset = reset;
    program_counter_write_enable = pc_write_enable;
    program_counter_next = pc_data;
    /*program_counter.tock(reset, pc_write_enable, pc_data)*/;

    igen_inst = inst;
    mux_reg_writeback_sel = reg_writeback_select;
    mux_reg_writeback_in0 = alu_result2;
    mux_reg_writeback_in1 = data_mem_read_data;
    mux_reg_writeback_in2 = pc_plus_4;
    mux_reg_writeback_in3 = igen_immediate;
    mux_reg_writeback_in4 = 32'b0;
    mux_reg_writeback_in5 = 32'b0;
    mux_reg_writeback_in6 = 32'b0;
    mux_reg_writeback_in7 = 32'b0;
    reg_data = mux_reg_writeback_out;
    idec_inst = inst;
    regs_write_enable = regfile_write_enable;
    regs_rd_address = idec_inst_rd;
    regs_rd_data = reg_data;
    /*regs.tock(regfile_write_enable, idec.inst_rd(inst), reg_data)*/;
  end


  //----------------------------------------

 /*private:*/
  adder #(32) adder_pc_plus_4(
    // Inputs
    .clock(clock),
    .operand_a(adder_pc_plus_4_operand_a), 
    .operand_b(adder_pc_plus_4_operand_b), 
    // Outputs
    .adder_result(adder_pc_plus_4_adder_result)
  );
  logic[32-1:0] adder_pc_plus_4_operand_a;
  logic[32-1:0] adder_pc_plus_4_operand_b;
  logic[32-1:0] adder_pc_plus_4_adder_result;

  adder #(32) adder_pc_plus_immediate(
    // Inputs
    .clock(clock),
    .operand_a(adder_pc_plus_immediate_operand_a), 
    .operand_b(adder_pc_plus_immediate_operand_b), 
    // Outputs
    .adder_result(adder_pc_plus_immediate_adder_result)
  );
  logic[32-1:0] adder_pc_plus_immediate_operand_a;
  logic[32-1:0] adder_pc_plus_immediate_operand_b;
  logic[32-1:0] adder_pc_plus_immediate_adder_result;

  alu alu_core(
    // Inputs
    .clock(clock),
    .alu_function(alu_core_alu_function), 
    .operand_a(alu_core_operand_a), 
    .operand_b(alu_core_operand_b), 
    // Outputs
    .alu_result(alu_core_alu_result)
  );
  logic[4:0] alu_core_alu_function;
  logic[31:0] alu_core_operand_a;
  logic[31:0] alu_core_operand_b;
  logic[31:0] alu_core_alu_result;

  multiplexer4 #(32) mux_next_pc_select(
    // Inputs
    .clock(clock),
    .sel(mux_next_pc_select_sel), 
    .in0(mux_next_pc_select_in0), 
    .in1(mux_next_pc_select_in1), 
    .in2(mux_next_pc_select_in2), 
    .in3(mux_next_pc_select_in3), 
    // Outputs
    .out(mux_next_pc_select_out)
  );
  logic[1:0] mux_next_pc_select_sel;
  logic[32-1:0] mux_next_pc_select_in0;
  logic[32-1:0] mux_next_pc_select_in1;
  logic[32-1:0] mux_next_pc_select_in2;
  logic[32-1:0] mux_next_pc_select_in3;
  logic[32-1:0] mux_next_pc_select_out;

  multiplexer2 #(32) mux_operand_a(
    // Inputs
    .clock(clock),
    .sel(mux_operand_a_sel), 
    .in0(mux_operand_a_in0), 
    .in1(mux_operand_a_in1), 
    // Outputs
    .out(mux_operand_a_out)
  );
  logic mux_operand_a_sel;
  logic[32-1:0] mux_operand_a_in0;
  logic[32-1:0] mux_operand_a_in1;
  logic[32-1:0] mux_operand_a_out;

  multiplexer2 #(32) mux_operand_b(
    // Inputs
    .clock(clock),
    .sel(mux_operand_b_sel), 
    .in0(mux_operand_b_in0), 
    .in1(mux_operand_b_in1), 
    // Outputs
    .out(mux_operand_b_out)
  );
  logic mux_operand_b_sel;
  logic[32-1:0] mux_operand_b_in0;
  logic[32-1:0] mux_operand_b_in1;
  logic[32-1:0] mux_operand_b_out;

  multiplexer8 #(32) mux_reg_writeback(
    // Inputs
    .clock(clock),
    .sel(mux_reg_writeback_sel), 
    .in0(mux_reg_writeback_in0), 
    .in1(mux_reg_writeback_in1), 
    .in2(mux_reg_writeback_in2), 
    .in3(mux_reg_writeback_in3), 
    .in4(mux_reg_writeback_in4), 
    .in5(mux_reg_writeback_in5), 
    .in6(mux_reg_writeback_in6), 
    .in7(mux_reg_writeback_in7), 
    // Outputs
    .out(mux_reg_writeback_out)
  );
  logic[2:0] mux_reg_writeback_sel;
  logic[32-1:0] mux_reg_writeback_in0;
  logic[32-1:0] mux_reg_writeback_in1;
  logic[32-1:0] mux_reg_writeback_in2;
  logic[32-1:0] mux_reg_writeback_in3;
  logic[32-1:0] mux_reg_writeback_in4;
  logic[32-1:0] mux_reg_writeback_in5;
  logic[32-1:0] mux_reg_writeback_in6;
  logic[32-1:0] mux_reg_writeback_in7;
  logic[32-1:0] mux_reg_writeback_out;

  single_register #(32, INITIAL_PC) program_counter(
    // Inputs
    .clock(clock),
    .reset(program_counter_reset), 
    .write_enable(program_counter_write_enable), 
    .next(program_counter_next), 
    // Outputs
    .value(program_counter_value)
  );
  logic program_counter_reset;
  logic program_counter_write_enable;
  logic[32-1:0] program_counter_next;
  logic[32-1:0] program_counter_value;

  regfile regs(
    // Inputs
    .clock(clock),
    .rs1_address(regs_rs1_address), 
    .rs2_address(regs_rs2_address), 
    .write_enable(regs_write_enable), 
    .rd_address(regs_rd_address), 
    .rd_data(regs_rd_data), 
    // Outputs
    .rs1_data(regs_rs1_data), 
    .rs2_data(regs_rs2_data)
  );
  logic[4:0] regs_rs1_address;
  logic[4:0] regs_rs2_address;
  logic regs_write_enable;
  logic[4:0] regs_rd_address;
  logic[31:0] regs_rd_data;
  logic[31:0] regs_rs1_data;
  logic[31:0] regs_rs2_data;

  instruction_decoder idec(
    // Inputs
    .clock(clock),
    .inst(idec_inst), 
    // Outputs
    .inst_opcode(idec_inst_opcode), 
    .inst_funct3(idec_inst_funct3), 
    .inst_funct7(idec_inst_funct7), 
    .inst_rd(idec_inst_rd), 
    .inst_rs1(idec_inst_rs1), 
    .inst_rs2(idec_inst_rs2)
  );
  logic[31:0] idec_inst;
  logic[6:0] idec_inst_opcode;
  logic[2:0] idec_inst_funct3;
  logic[6:0] idec_inst_funct7;
  logic[4:0] idec_inst_rd;
  logic[4:0] idec_inst_rs1;
  logic[4:0] idec_inst_rs2;

  immediate_generator igen(
    // Inputs
    .clock(clock),
    .inst(igen_inst), 
    // Outputs
    .immediate(igen_immediate)
  );
  logic[31:0] igen_inst;
  logic[31:0] igen_immediate;

endmodule;

`endif  // RVSIMPLE_SINGLECYCLE_DATAPATH_H

