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

  logic<32> pc;
  logic<1> bus_write_enable2;
  logic<4> bus_byte_enable2;
  logic<1> bus_read_enable2;

  void tock_pc() { pc = datapath.pc(); }

  void tock_inst() {
    datapath.inst = inst;
    datapath.tock_inst();
  }

  void tock_alu_result() {
    logic<7> opcode = datapath.inst_opcode;
    logic<3> funct3 = datapath.inst_funct3;
    logic<7> funct7 = datapath.inst_funct7;

    ctlpath.inst_opcode = opcode;
    ctlpath.inst_funct3 = funct3;
    ctlpath.inst_funct7 = funct7;
    ctlpath.tock_alu_function();
    ctlpath.tock_alu_operand_a_select();
    ctlpath.tock_alu_operand_b_select();

    datapath.alu_function = ctlpath.alu_function;
    datapath.alu_operand_a_select = ctlpath.alu_operand_a_select;
    datapath.alu_operand_b_select = ctlpath.alu_operand_b_select;
    datapath.tock_alu_result();
    alu_result = datapath.alu_result;

    dmem.address = alu_result;
    dmem.data_format = funct3;
    dmem.tock_inputs();
  }

  void tock() {
    ctlpath.alu_result_equal_zero = alu_result == 0;
    ctlpath.tock_regfile_write_enable();
    ctlpath.tock_reg_writeback_select();
    ctlpath.tock_next_pc_select();
    ctlpath.tock_pc_write_enable();

    dmem.bus_read_data = bus_read_data;
    dmem.tock_read_data();

    datapath.reset = reset;
    datapath.regfile_write_enable = ctlpath.regfile_write_enable;
    datapath.data_mem_read_data = dmem.read_data;
    datapath.reg_writeback_select = ctlpath.reg_writeback_select;
    datapath.next_pc_select = ctlpath.next_pc_select;
    datapath.pc_write_enable = ctlpath.pc_write_enable;
    datapath.tock();
  }

  void tock_bus_write_data2() {
    dmem.write_data = datapath.temp_rs2_data;
    dmem.tock_bus_write_data();
    bus_write_data = dmem.bus_write_data;
  }

  void tock_bus_write_enable2() {
    ctlpath.tock_data_mem_write_enable();
    bus_write_enable2 = ctlpath.data_mem_write_enable;
  }

  void tock_bus_byte_enable2() {
    logic<3> funct3 = datapath.inst_funct3;
    dmem.tock_bus_byte_enable();
    bus_byte_enable2 = dmem.bus_byte_enable;
  }

  void tock_bus_read_enable2() {
    ctlpath.tock_data_mem_read_enable();
    bus_read_enable2 = ctlpath.data_mem_read_enable;
  }

  //----------------------------------------

 private:
  singlecycle_datapath datapath;
  singlecycle_ctlpath ctlpath;
  data_memory_interface dmem;
};

#endif  // RVSIMPLE_RISCV_CORE_H
