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
  //----------------------------------------

  logic<32> pc() const { return program_counter.value(); }

  //----------------------------------------

  logic<1> reset;
  logic<32> inst;
  logic<1> regfile_write_enable;
  logic<32> data_mem_read_data;
  logic<3> reg_writeback_select;
  logic<2> next_pc_select;
  logic<1> pc_write_enable;

  logic<5> inst_rd;
  logic<5> inst_rs1;
  logic<5> inst_rs2;
  logic<7> inst_opcode;
  logic<3> inst_funct3;
  logic<7> inst_funct7;
  logic<32> inst_immediate;

  void tock_inst() {
    idec.inst = inst;
    idec.tock();

    inst_opcode = idec.inst_opcode;
    inst_funct3 = idec.inst_funct3;
    inst_funct7 = idec.inst_funct7;
    inst_rd = idec.inst_rd;
    inst_rs1 = idec.inst_rs1;
    inst_rs2 = idec.inst_rs2;

    igen.inst = inst;
    igen.tock_immediate();
    inst_immediate = igen.immediate;
  }

  //----------------------------------------

  logic<32> alu_result;
  logic<5> alu_function;
  logic<1> alu_operand_a_select;
  logic<1> alu_operand_b_select;

  logic<32> temp_rs1_data;
  logic<32> temp_rs2_data;

  void tock_alu_result() {

    regs.rs1_address = inst_rs1;
    regs.rs2_address = inst_rs2;

    regs.tock_rs1_data();
    regs.tock_rs2_data();

    temp_rs1_data = regs.rs1_data;
    temp_rs2_data = regs.rs2_data;

    mux_operand_a.sel = alu_operand_a_select;
    mux_operand_a.in0 = regs.rs1_data;
    mux_operand_a.in1 = program_counter.value();
    mux_operand_a.tock();

    mux_operand_b.sel = alu_operand_b_select;
    mux_operand_b.in0 = regs.rs2_data;
    mux_operand_b.in1 = inst_immediate;
    mux_operand_b.tock();

    alu_core.alu_function = alu_function;
    alu_core.operand_a = mux_operand_a.out;
    alu_core.operand_b = mux_operand_b.out;
    alu_core.tock();

    alu_result = alu_core.result;
  }

  //----------------------------------------

  void tock() {
    adder_pc_plus_4.operand_a = b32(0x00000004);
    adder_pc_plus_4.operand_b = program_counter.value();
    adder_pc_plus_4.tock();

    adder_pc_plus_immediate.operand_a = program_counter.value();
    adder_pc_plus_immediate.operand_b = inst_immediate;
    adder_pc_plus_immediate.tock();

    mux_next_pc_select.sel = next_pc_select;
    mux_next_pc_select.in0 = adder_pc_plus_4.result;
    mux_next_pc_select.in1 = adder_pc_plus_immediate.result;
    mux_next_pc_select.in2 = cat(b31(alu_result, 1), b1(0b0));
    mux_next_pc_select.in3 = b32(0b0);
    mux_next_pc_select.tock();

    program_counter.tock(reset, pc_write_enable, mux_next_pc_select.out);

    mux_reg_writeback.sel = reg_writeback_select;

    mux_reg_writeback.in0 = alu_result;
    mux_reg_writeback.in1 = data_mem_read_data;
    mux_reg_writeback.in2 = adder_pc_plus_4.result;
    mux_reg_writeback.in3 = inst_immediate;
    mux_reg_writeback.in4 = b32(0b0);
    mux_reg_writeback.in5 = b32(0b0);
    mux_reg_writeback.in6 = b32(0b0);
    mux_reg_writeback.in7 = b32(0b0);
    mux_reg_writeback.tock();

    regs.write_enable = regfile_write_enable;
    regs.rd_address = inst_rd;
    regs.rd_data = mux_reg_writeback.out;
    regs.tock();
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
