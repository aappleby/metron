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

  void init() { program_counter.init(); }

  //----------------------------------------

  logic<32> pc() const {
    return program_counter.value;
  }

  logic<32> rs2_data(logic<32> inst) const {
    logic<5> rs2_idx = idec.inst_rs2(inst);
    return regs.rs2_data(rs2_idx);
  }

  logic<7>  inst_opcode(logic<32> inst) const { return idec.inst_opcode(inst); }
  logic<3>  inst_funct3(logic<32> inst) const { return idec.inst_funct3(inst); }
  logic<7>  inst_funct7(logic<32> inst) const { return idec.inst_funct7(inst); }

  //----------------------------------------

  logic<32> alu_result(logic<32> inst, logic<5> alu_function, logic<1> alu_operand_a_select, logic<1> alu_operand_b_select) const {
    logic<5> rs1_idx = idec.inst_rs1(inst);
    logic<5> rs2_idx = idec.inst_rs2(inst);

    return alu_core.result(
      alu_function,
      mux_operand_a.out(
        alu_operand_a_select,
        regs.rs1_data(rs1_idx),
        program_counter.value
      ),
      mux_operand_b.out(
        alu_operand_b_select,
        regs.rs2_data(rs2_idx),
        igen.immediate(inst)
      )
    );
  }

  void tock(
    logic<1> reset,
    logic<32> inst,
    logic<1> regfile_write_enable,
    logic<32> data_mem_read_data,
    logic<3> reg_writeback_select,
    logic<32> alu_result2,
    logic<2> next_pc_select,
    logic<1> pc_write_enable
  ) {
    logic<32> pc_plus_4 = adder_pc_plus_4.result(b32(0x00000004), program_counter.value);
    logic<32> pc_plus_imm = adder_pc_plus_immediate.result(program_counter.value, igen.immediate(inst));

    logic<32> pc_data = mux_next_pc_select.out(
      next_pc_select,
      pc_plus_4,
      pc_plus_imm,
      cat(b31(alu_result2, 1), b1(0b0)),
      b32(0b0)
    );
    program_counter.tock(reset, pc_write_enable, pc_data);

    logic<32> reg_data = mux_reg_writeback.out(
      reg_writeback_select,
      alu_result2,
      data_mem_read_data,
      pc_plus_4,
      igen.immediate(inst),
      b32(0b0),
      b32(0b0),
      b32(0b0),
      b32(0b0)
    );
    regs.tock(regfile_write_enable, idec.inst_rd(inst), reg_data);
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
