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

   logic<32> inst;
   logic<32> alu_result;
   logic<1> reset;
   logic<32> bus_read_data;
   logic<32> bus_write_data;

  logic<32> pc() const { return datapath.pc(); }

  void tock_inst() {
    datapath.inst = inst;
    datapath.tock_inst();
  }

  logic<32> tock_alu_result() {
    logic<7> opcode = datapath.inst_opcode;
    logic<3> funct3 = datapath.inst_funct3;
    logic<7> funct7 = datapath.inst_funct7;

    ctlpath.inst_opcode = opcode;
    ctlpath.inst_funct3 = funct3;

    ctlpath.inst_funct7 = funct7;
    logic<5> alu_function = ctlpath.tock_alu_function();

    datapath.alu_function = alu_function;
    datapath.alu_operand_a_select = ctlpath.alu_operand_a_select();
    datapath.alu_operand_b_select = ctlpath.alu_operand_b_select();

    datapath.tock_alu_result();
    alu_result = datapath.alu_result;

    dmem.address = alu_result;
    dmem.data_format = funct3;
    dmem.tock_inputs();
    return alu_result;
  }

  void tock() {
    logic<7> opcode = datapath.inst_opcode;
    logic<3> funct3 = datapath.inst_funct3;
    logic<1> reg_we = ctlpath.regfile_write_enable();

    dmem.bus_read_data = bus_read_data;
    logic<32> mem_data = dmem.read_data();
    logic<3> reg_select = ctlpath.reg_writeback_select();

    ctlpath.alu_result_equal_zero = alu_result == 0;
    logic<2> pc_select = ctlpath.next_pc_select();
    logic<1> pc_we = ctlpath.pc_write_enable();

    datapath.reset = reset;
    datapath.regfile_write_enable = reg_we;
    datapath.data_mem_read_data = mem_data;
    datapath.reg_writeback_select = reg_select;
    datapath.next_pc_select = pc_select;
    datapath.pc_write_enable = pc_we;
    datapath.tock();
  }


  void tock_bus_write_data2() {
    dmem.write_data = datapath.temp_rs2_data;
    bus_write_data = dmem.bus_write_data();
  }

  logic<1> bus_write_enable2() const {
    return ctlpath.data_mem_write_enable();
  }

  logic<4> bus_byte_enable2() const {
    logic<3> funct3 = datapath.inst_funct3;
    return dmem.bus_byte_enable();
  }

  logic<1> bus_read_enable2() const {
    return ctlpath.data_mem_read_enable();
  }

  //----------------------------------------

 private:
  singlecycle_datapath datapath;
  singlecycle_ctlpath ctlpath;
  data_memory_interface dmem;
};

#endif  // RVSIMPLE_RISCV_CORE_H
