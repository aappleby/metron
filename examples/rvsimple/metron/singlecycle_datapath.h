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

  logic<32> pc() const {
    return program_counter.value();
  }

  //----------------------------------------

  logic<32> inst;

  logic<5>  inst_rd;
  logic<5>  inst_rs1;
  logic<5>  inst_rs2;
  logic<7>  inst_opcode;
  logic<3>  inst_funct3;
  logic<7>  inst_funct7;
  logic<32> inst_immediate;

  void tock_inst() {
    idec.inst = inst;

    inst_opcode    = idec.inst_opcode();
    inst_funct3    = idec.inst_funct3();
    inst_funct7    = idec.inst_funct7();
    inst_rd        = idec.inst_rd();
    inst_rs1       = idec.inst_rs1();
    inst_rs2       = idec.inst_rs2();
    inst_immediate = igen.immediate(inst);
  }

  //----------------------------------------

  logic<32> temp_rs1_data;
  logic<32> temp_rs2_data;

  logic<32> tock_alu_result(logic<5> alu_function, logic<1> alu_operand_a_select, logic<1> alu_operand_b_select) {
    temp_rs1_data = regs.rs1_data(inst_rs1);
    temp_rs2_data = regs.rs2_data(inst_rs2);

    return alu_core.alu_result(
      alu_function,
      mux_operand_a.out(
        alu_operand_a_select,
        temp_rs1_data,
        program_counter.value()
      ),
      mux_operand_b.out(
        alu_operand_b_select,
        temp_rs2_data,
        inst_immediate
      )
    );
  }

  //----------------------------------------

  void tock(
    logic<1> reset,
    logic<1> regfile_write_enable,
    logic<32> data_mem_read_data,
    logic<3> reg_writeback_select,
    logic<32> alu_result2,
    logic<2> next_pc_select,
    logic<1> pc_write_enable
  ) {
    logic<32> pc_plus_4 = adder_pc_plus_4.adder_result(b32(0x00000004), program_counter.value());
    logic<32> pc_plus_imm = adder_pc_plus_immediate.adder_result(program_counter.value(), inst_immediate);

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
      inst_immediate,
      b32(0b0),
      b32(0b0),
      b32(0b0),
      b32(0b0)
    );
    regs.tock(regfile_write_enable, inst_rd, reg_data);
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
