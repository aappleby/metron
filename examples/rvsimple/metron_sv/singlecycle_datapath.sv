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
  input logic reset,
  input logic[31:0] inst,
  input logic regfile_write_enable,
  input logic[31:0] data_mem_read_data,
  input logic[2:0] reg_writeback_select,
  input logic[1:0] next_pc_select,
  input logic pc_write_enable,
  input logic[4:0] alu_function,
  input logic alu_operand_a_select,
  input logic alu_operand_b_select,
  output logic[6:0] inst_opcode,
  output logic[2:0] inst_funct3,
  output logic[6:0] inst_funct7,
  output logic[31:0] alu_result,
  output logic[31:0] temp_rs1_data,
  output logic[31:0] temp_rs2_data,
  output logic[31:0] pc
);
 /*public:*/
  //----------------------------------------

  always_comb begin pc = program_counter_value; end

  //----------------------------------------

  /*logic<1> reset;*/
  /*logic<32> inst;*/
  /*logic<1> regfile_write_enable;*/
  /*logic<32> data_mem_read_data;*/
  /*logic<3> reg_writeback_select;*/
  /*logic<2> next_pc_select;*/
  /*logic<1> pc_write_enable;*/

  /*logic<7> inst_opcode;*/
  /*logic<3> inst_funct3;*/
  /*logic<7> inst_funct7;*/
  /*logic<32> alu_result;*/
  /*logic<5> alu_function;*/
  /*logic<1> alu_operand_a_select;*/
  /*logic<1> alu_operand_b_select;*/

  /*logic<32> temp_rs1_data;*/
  /*logic<32> temp_rs2_data;*/


  always_comb begin /*tock1*/
    idec_inst = inst;
    /*idec.tock()*/;

    inst_opcode = idec_inst_opcode;
    inst_funct3 = idec_inst_funct3;
    inst_funct7 = idec_inst_funct7;

    igen_inst = inst;
    /*igen.tock()*/;
  end

  //----------------------------------------

  always_comb begin /*tock2*/

    regs_rs1_address = idec_inst_rs1;
    regs_rs2_address = idec_inst_rs2;

    /*regs.tock1()*/;

    temp_rs1_data = regs_rs1_data;
    temp_rs2_data = regs_rs2_data;

    mux_operand_a_sel = alu_operand_a_select;
    mux_operand_a_in0 = regs_rs1_data;
    mux_operand_a_in1 = program_counter_value;
    /*mux_operand_a.tock()*/;

    mux_operand_b_sel = alu_operand_b_select;
    mux_operand_b_in0 = regs_rs2_data;
    mux_operand_b_in1 = igen_immediate;
    /*mux_operand_b.tock()*/;

    alu_core_alu_function = alu_function;
    alu_core_operand_a = mux_operand_a_out;
    alu_core_operand_b = mux_operand_b_out;
    /*alu_core.tock()*/;

    alu_result = alu_core_result;
  end

  //----------------------------------------

  always_comb begin /*tock3*/
    adder_pc_plus_4_operand_a = 32'h00000004;
    adder_pc_plus_4_operand_b = program_counter_value;
    /*adder_pc_plus_4.tock()*/;

    adder_pc_plus_immediate_operand_a = program_counter_value;
    adder_pc_plus_immediate_operand_b = igen_immediate;
    /*adder_pc_plus_immediate.tock()*/;

    mux_next_pc_select_sel = next_pc_select;
    mux_next_pc_select_in0 = adder_pc_plus_4_result;
    mux_next_pc_select_in1 = adder_pc_plus_immediate_result;
    mux_next_pc_select_in2 = {alu_result[31:1], 1'b0};
    mux_next_pc_select_in3 = 32'b0;
    /*mux_next_pc_select.tock()*/;

    program_counter_reset = reset;
    program_counter_write_enable = pc_write_enable;
    program_counter_next = mux_next_pc_select_out;
    /*program_counter.tock()*/;

    mux_reg_writeback_sel = reg_writeback_select;
    mux_reg_writeback_in0 = alu_result;
    mux_reg_writeback_in1 = data_mem_read_data;
    mux_reg_writeback_in2 = adder_pc_plus_4_result;
    mux_reg_writeback_in3 = igen_immediate;
    mux_reg_writeback_in4 = 32'b0;
    mux_reg_writeback_in5 = 32'b0;
    mux_reg_writeback_in6 = 32'b0;
    mux_reg_writeback_in7 = 32'b0;
    /*mux_reg_writeback.tock()*/;

    regs_write_enable = regfile_write_enable;
    regs_rd_address = idec_inst_rd;
    regs_rd_data = mux_reg_writeback_out;
    /*regs.tock2()*/;
  end

  //----------------------------------------

 /*private:*/
  adder #(32) adder_pc_plus_4(
    // Inputs
    .clock(clock),
    .operand_a(adder_pc_plus_4_operand_a), 
    .operand_b(adder_pc_plus_4_operand_b), 
    // Outputs
    .result(adder_pc_plus_4_result)
  );
  logic[32-1:0] adder_pc_plus_4_operand_a;
  logic[32-1:0] adder_pc_plus_4_operand_b;
  logic[32-1:0] adder_pc_plus_4_result;

  adder #(32) adder_pc_plus_immediate(
    // Inputs
    .clock(clock),
    .operand_a(adder_pc_plus_immediate_operand_a), 
    .operand_b(adder_pc_plus_immediate_operand_b), 
    // Outputs
    .result(adder_pc_plus_immediate_result)
  );
  logic[32-1:0] adder_pc_plus_immediate_operand_a;
  logic[32-1:0] adder_pc_plus_immediate_operand_b;
  logic[32-1:0] adder_pc_plus_immediate_result;

  alu alu_core(
    // Inputs
    .clock(clock),
    .alu_function(alu_core_alu_function), 
    .operand_a(alu_core_operand_a), 
    .operand_b(alu_core_operand_b), 
    // Outputs
    .result(alu_core_result), 
    .result_equal_zero(alu_core_result_equal_zero)
  );
  logic[4:0] alu_core_alu_function;
  logic[31:0] alu_core_operand_a;
  logic[31:0] alu_core_operand_b;
  logic[31:0] alu_core_result;
  logic alu_core_result_equal_zero;

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

