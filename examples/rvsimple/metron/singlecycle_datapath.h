// RISC-V SiMPLE SV -- single-cycle data path
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

#ifndef RVSIMPLE_SINGLECYCLE_DATAPATH_H
#define RVSIMPLE_SINGLECYCLE_DATAPATH_H

#include "adder.h"
#include "alu.h"
#include "config.h"
#include "constants.h"
#include "immediate_generator.h"
#include "instruction_decoder.h"
#include "metron_tools.h"
#include "multiplexer2.h"
#include "multiplexer4.h"
#include "multiplexer8.h"
#include "regfile.h"
#include "register.h"

using namespace rv_config;

class singlecycle_datapath {
public:

  logic<1>  reset;
  logic<32> data_mem_read_data;
  logic<32> data_mem_address;
  logic<32> data_mem_write_data;

  logic<32> inst;
  logic<32> pc;
  logic<7>  inst_opcode;
  logic<3>  inst_funct3;
  logic<7>  inst_funct7;
  logic<1>  alu_result_equal_zero;

  // control signals
  logic<1>  pc_write_enable;
  logic<1>  regfile_write_enable;
  logic<1>  alu_operand_a_select;
  logic<1>  alu_operand_b_select;
  logic<3>  reg_writeback_select;
  logic<2>  next_pc_select;
  logic<5>  alu_function;

#if 0
  // register file inputs and outputs
  logic [31:0]  rd_data;
  logic [31:0]  rs1_data;
  logic [31:0]  rs2_data;
  logic  [4:0]  inst_rd;
  logic  [4:0]  inst_rs1;
  logic  [4:0]  inst_rs2;

  // program counter signals
  logic [31:0] pc_plus_4;
  logic [31:0] pc_plus_immediate;
  logic [31:0] next_pc;

  // ALU signals
  logic [31:0] alu_operand_a;
  logic [31:0] alu_operand_b;
  logic [31:0] alu_result;

  // immediate
  logic [31:0] immediate;
#endif

private:
  logic<32> rs1_data;
  logic<32> rs2_data;

  logic<5> inst_rd;
  logic<5> inst_rs1;
  logic<5> inst_rs2;
public:

  //----------------------------------------

  void tock_pc() {
    pc = program_counter.value;
  }

  //----------------------------------------

  void tock_instruction_decoder() {
    idec.inst = inst;
    idec.tock();

    inst_opcode = idec.inst_opcode;
    inst_funct3 = idec.inst_funct3;
    inst_funct7 = idec.inst_funct7;
    inst_rd     = idec.inst_rd;
    inst_rs1    = idec.inst_rs1;
    inst_rs2    = idec.inst_rs2;
  }

  //----------------------------------------

  void tock_immediate_generator() {
    igen.inst = inst;
    igen.tock();
  }

  //----------------------------------------

  void tock_reg_read() {
    regs.rd_address  = idec.inst_rd;
    regs.rs1_address = idec.inst_rs1;
    regs.rs2_address = idec.inst_rs2;
    regs.tock1();
    rs1_data = regs.rs1_data;
    rs2_data = regs.rs2_data;
  }

  void tock_mux_operand_a() {
    mux_operand_a.sel = alu_operand_a_select;
    mux_operand_a.in0 = regs.rs1_data;
    mux_operand_a.in1 = program_counter.value;
    mux_operand_a.tock();
  }

  void tock_mux_operand_b() {
    mux_operand_b.sel = alu_operand_b_select;
    mux_operand_b.in0 = regs.rs2_data;
    mux_operand_b.in1 = igen.immediate;
    mux_operand_b.tock();
  }

  void tock_alu() {
    alu_core.alu_function = alu_function;
    alu_core.operand_a = mux_operand_a.out;
    alu_core.operand_b = mux_operand_b.out;
    alu_core.tock();
    alu_result_equal_zero = alu_core.result_equal_zero;
  }

  void tock_adder_pc_plus_4() {
    adder_pc_plus_4.operand_a = b32(0x00000004);
    adder_pc_plus_4.operand_b = program_counter.value;
    adder_pc_plus_4.tock();
  }

  void tock_adder_pc_plus_immediate() {
    adder_pc_plus_immediate.operand_a = program_counter.value;
    adder_pc_plus_immediate.operand_b = igen.immediate;
    adder_pc_plus_immediate.tock();
  }

  void tock_data_mem_write_data() {
    data_mem_address    = alu_core.result;
    data_mem_write_data = regs.rs2_data;
  }

  void tock_mux_next_pc_select() {
    mux_next_pc_select.sel = next_pc_select;
    mux_next_pc_select.in0 = adder_pc_plus_4.result;
    mux_next_pc_select.in1 = adder_pc_plus_immediate.result;
    mux_next_pc_select.in2 = cat(b31(alu_core.result, 1), b1(0b0));
    mux_next_pc_select.in3 = b32(0b0);
    mux_next_pc_select.tock();
  }

  void tock_program_counter() {
    program_counter.reset = reset;
    program_counter.write_enable = pc_write_enable;
    program_counter.next = mux_next_pc_select.out;
    program_counter.tick();
  }

  void tock_mux_reg_writeback() {
    mux_reg_writeback.sel = reg_writeback_select;
    mux_reg_writeback.in0 = alu_core.result;
    mux_reg_writeback.in1 = data_mem_read_data;
    mux_reg_writeback.in2 = adder_pc_plus_4.result;
    mux_reg_writeback.in3 = igen.immediate;
    mux_reg_writeback.in4 = b32(0b0);
    mux_reg_writeback.in5 = b32(0b0);
    mux_reg_writeback.in6 = b32(0b0);
    mux_reg_writeback.in7 = b32(0b0);
    mux_reg_writeback.tock();
  }

  void tock_reg_writeback() {
    regs.write_enable = regfile_write_enable;
    regs.rd_data = mux_reg_writeback.out;
    regs.tock2();
  }

  //----------------------------------------

 private:
  adder<32> adder_pc_plus_4;
  adder<32> adder_pc_plus_immediate;
  alu alu_core;
  multiplexer4<32> mux_next_pc_select;
  multiplexer2<32> mux_operand_a;
  multiplexer2<32> mux_operand_b;
  multiplexer8<32> mux_reg_writeback;
  single_register<32, INITIAL_PC> program_counter;
  regfile regs;
  instruction_decoder idec;
  immediate_generator igen;
};

#endif  // RVSIMPLE_SINGLECYCLE_DATAPATH_H
