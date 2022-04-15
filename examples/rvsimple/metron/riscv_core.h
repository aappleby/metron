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
  logic<1>  reset;
  logic<32> bus_address;
  logic<32> bus_read_data;
  logic<32> bus_write_data;
  logic<4>  bus_byte_enable;
  logic<1>  bus_read_enable;
  logic<1>  bus_write_enable;

  logic<32> inst;
  logic<32> pc; 

  void tock_pc() {
    datapath.tock_pc();
    pc = datapath.pc;
  }

  void tock_execute() {
    datapath.inst = inst;
    datapath.tock_instruction_decoder();
    datapath.tock_immediate_generator();

    ctlpath.inst_opcode = datapath.inst_opcode;
    ctlpath.inst_funct3 = datapath.inst_funct3;
    ctlpath.inst_funct7 = datapath.inst_funct7;

    ctlpath.tock_alu_function();
    ctlpath.tock_alu_operand_select();

    datapath.alu_function         = ctlpath.alu_function;
    datapath.alu_operand_a_select = ctlpath.alu_operand_a_select;
    datapath.alu_operand_b_select = ctlpath.alu_operand_b_select;
    
    datapath.tock_reg_read();
    datapath.tock_mux_operand_a();
    datapath.tock_mux_operand_b();
    datapath.tock_alu();

    ctlpath.tock_data_mem_enable();
    datapath.tock_data_mem_write_data();
    dmem.read_enable  = ctlpath.data_mem_read_enable;
    dmem.write_enable = ctlpath.data_mem_write_enable;
    dmem.data_format  = datapath.inst_funct3;
    dmem.address      = datapath.data_mem_address;
    dmem.write_data   = datapath.data_mem_write_data;
    dmem.tock_bus();

    ctlpath.alu_result_equal_zero = datapath.alu_result_equal_zero;
    ctlpath.tock_pc_write_enable();
    ctlpath.tock_regfile_write_enable();
    ctlpath.tock_reg_writeback_select();
    ctlpath.tock_next_pc_select();

    //----------

    bus_address      = dmem.bus_address;
    bus_write_data   = dmem.bus_write_data;
    bus_byte_enable  = dmem.bus_byte_enable;
    bus_read_enable  = dmem.bus_read_enable;
    bus_write_enable = dmem.bus_write_enable;
  }

  void tock_writeback() {
    dmem.bus_read_data = bus_read_data;
    dmem.tock_read_data();

    datapath.next_pc_select       = ctlpath.next_pc_select;
    datapath.tock_adder_pc_plus_4();
    datapath.tock_adder_pc_plus_immediate();
    datapath.tock_mux_next_pc_select();

    datapath.reset                = reset;
    datapath.pc_write_enable      = ctlpath.pc_write_enable;
    datapath.tock_program_counter();

    datapath.reg_writeback_select = ctlpath.reg_writeback_select;
    datapath.data_mem_read_data   = dmem.read_data;
    datapath.tock_mux_reg_writeback();

    datapath.regfile_write_enable = ctlpath.regfile_write_enable;
    datapath.tock_reg_writeback();
  }

  //----------------------------------------

 private:
  singlecycle_datapath  datapath;
  singlecycle_ctlpath   ctlpath;
  data_memory_interface dmem;
};

#endif  // RVSIMPLE_RISCV_CORE_H
