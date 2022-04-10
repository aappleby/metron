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

  void tock1() {
    datapath.inst = inst;
    datapath.tock_inst();

    ctlpath.inst_opcode = datapath.inst_opcode;
    ctlpath.inst_funct3 = datapath.inst_funct3;
    ctlpath.inst_funct7 = datapath.inst_funct7;
    ctlpath.tock1();

    datapath.alu_function = ctlpath.alu_function;
    datapath.alu_operand_a_select = ctlpath.alu_operand_a_select;
    datapath.alu_operand_b_select = ctlpath.alu_operand_b_select;
    datapath.tock_alu_result();

    dmem.address = datapath.alu_result;
    dmem.data_format = datapath.inst_funct3;
    dmem.write_data = datapath.temp_rs2_data;
    dmem.tock_inputs();
    dmem.tock_bus_write_data();
    dmem.tock_bus_byte_enable();

    bus_write_data    = dmem.bus_write_data;
    bus_write_enable2 = ctlpath.data_mem_write_enable;
    bus_byte_enable2  = dmem.bus_byte_enable;
    bus_read_enable2  = ctlpath.data_mem_read_enable;

    alu_result = datapath.alu_result;
  }

  void tock2() {
    ctlpath.alu_result_equal_zero = alu_result == 0;
    ctlpath.tock2();

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

  //----------------------------------------

 private:
  singlecycle_datapath datapath;
  singlecycle_ctlpath ctlpath;
  data_memory_interface dmem;
};

#endif  // RVSIMPLE_RISCV_CORE_H
