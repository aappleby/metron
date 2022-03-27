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

  logic<32> pc2()                  const { return program_counter.value; }
  logic<32> data_mem_write_data2(logic<32> inst) const { return regs.rs2_data(idec.inst_rs22(inst)); }

  logic<7>  inst_opcode2(logic<32> inst) const { return idec.inst_opcode2(inst); }
  logic<3>  inst_funct32(logic<32> inst) const { return idec.inst_funct32(inst); }
  logic<7>  inst_funct72(logic<32> inst) const { return idec.inst_funct72(inst); }

  //----------------------------------------

  logic<32> alu_result(logic<32> inst, logic<5> alu_function, logic<1> alu_operand_a_select, logic<1> alu_operand_b_select) {
    return alu_core.result(
      alu_function,
      mux_operand_a.out(
        alu_operand_a_select,
        regs.rs1_data(idec.inst_rs12(inst)),
        program_counter.value
      ),
      mux_operand_b.out(
        alu_operand_b_select,
        regs.rs2_data(idec.inst_rs22(inst)),
        igen.immediate(inst)
      )
    );
  }

  void tocktick_regs(logic<1> reset, logic<32> inst, logic<1> regfile_write_enable, logic<32> data_mem_read_data, logic<3> reg_writeback_select, logic<32> alu_result2) {
    logic<32> out = mux_reg_writeback.out(
      reg_writeback_select,
      alu_result2,
      data_mem_read_data,
      adder_pc_plus_4.result(b32(0x00000004), program_counter.value),
      igen.immediate(inst),
      b32(0b0),
      b32(0b0),
      b32(0b0),
      b32(0b0)
    );
    regs.tick(regfile_write_enable, idec.inst_rd2(inst), out);
  }
  void tocktick_pc(logic<1> reset, logic<32> inst, logic<2> next_pc_select, logic<1> pc_write_enable, logic<32> alu_result2) {
    logic<32> out = mux_next_pc_select.out(
      next_pc_select,
      adder_pc_plus_4.result(b32(0x00000004), program_counter.value),
      adder_pc_plus_immediate.result(program_counter.value, igen.immediate(inst)),
      cat(b31(alu_result2, 1), b1(0b0)),
      b32(0b0)
    );
    program_counter.tick(reset, pc_write_enable, out);
  }


  //----------------------------------------

 private:
  //logic<32> pc;
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
