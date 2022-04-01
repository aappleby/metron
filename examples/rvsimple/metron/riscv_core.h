// RISC-V SiMPLE SV -- Single-cycle RISC-V core
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

#ifndef RVSIMPLE_RISCV_CORE_H
#define RVSIMPLE_RISCV_CORE_H

#include "config.h"
#include "constants.h"
#include "data_memory_interface.h"
#include "metron_tools.h"
#include "singlecycle_ctlpath.h"
#include "singlecycle_datapath.h"

class riscv_core {
 public:

  logic<32> bus_write_data2(logic<32> inst, logic<32> alu_result2) const {
    return dmem.bus_write_data(
      alu_result2,
      datapath.rs2_data(inst)
    );
  }

  logic<4>  bus_byte_enable2(logic<32> inst, logic<32> alu_result2)  const {
    logic<3> funct3 = datapath.inst_funct3(inst);
    return dmem.bus_byte_enable(
      funct3,
      alu_result2
    );
  }

  logic<1>  bus_read_enable2(logic<32> inst)  const {
    logic<7> opcode = datapath.inst_opcode(inst);
    return ctlpath.data_mem_read_enable(opcode);
  }

  logic<1>  bus_write_enable2(logic<32> inst) const {
    logic<7> opcode = datapath.inst_opcode(inst);
    return ctlpath.data_mem_write_enable(opcode);
  }

  logic<32> pc() const {
    return datapath.pc();
  }

  //----------------------------------------

  void init() {
    datapath.init();
  }

  logic<32> alu_result(logic<32> inst) const {
    logic<7> opcode = datapath.inst_opcode(inst);
    logic<3> funct3 = datapath.inst_funct3(inst);
    logic<7> funct7 = datapath.inst_funct7(inst);

    logic<5> alu_function = ctlpath.alu_function(opcode, funct3, funct7);

    return datapath.alu_result(
      inst,
      alu_function,
      ctlpath.alu_operand_a_select(opcode),
      ctlpath.alu_operand_b_select(opcode)
    );
  }

  void tock(logic<1> reset, logic<32> inst, logic<32> bus_read_data, logic<32> alu_result2) {
    logic<7> opcode = datapath.inst_opcode(inst);
    logic<3> funct3 = datapath.inst_funct3(inst);
    logic<1> reg_we = ctlpath.regfile_write_enable(opcode);

    logic<32> mem_data = dmem.read_data(alu_result2, bus_read_data, funct3);
    logic<3> reg_select = ctlpath.reg_writeback_select(opcode);
    logic<2> pc_select = ctlpath.next_pc_select(opcode, funct3, alu_result2 == 0);
    logic<1> pc_we = ctlpath.pc_write_enable(opcode);

    datapath.tock(
      reset,
      inst,
      reg_we,
      mem_data,
      reg_select,
      alu_result2,
      pc_select,
      pc_we
    );
  }


  //----------------------------------------

 private:
  singlecycle_datapath datapath;
  singlecycle_ctlpath ctlpath;
  data_memory_interface dmem;
};

#endif  // RVSIMPLE_RISCV_CORE_H
