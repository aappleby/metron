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
  /*I*/ logic<1>  reset;
  /*O*/ logic<32> bus_address;
  /*I*/ logic<32> bus_read_data;
  /*O*/ logic<32> bus_write_data;
  /*O*/ logic<4>  bus_byte_enable;
  /*O*/ logic<1>  bus_read_enable;
  /*O*/ logic<1>  bus_write_enable;

  /*I*/ logic<32> inst;
  /*O*/ logic<32> pc; 

  void tock_datapath_pc() {
    datapath.tock_pc();
    pc = datapath.pc;
  }

  void tock_datapath_decode() {
    datapath.inst = inst;
    datapath.tock_instruction_decoder();
    datapath.tock_immediate_generator();
  }

  void tock2a() {
    ctlpath.inst_opcode = datapath.inst_opcode;
    ctlpath.tock_control();

    ctlpath.inst_funct3 = datapath.inst_funct3;
    ctlpath.inst_funct7 = datapath.inst_funct7;
    ctlpath.tock1a();

    datapath.alu_function         = ctlpath.alu_function;
    datapath.alu_operand_a_select = ctlpath.alu_operand_a_select;
    datapath.alu_operand_b_select = ctlpath.alu_operand_b_select;
    
    datapath.tock_regs1();
    datapath.tock_mux_operand_a();
    datapath.tock_mux_operand_b();
    datapath.tock_alu();
    datapath.tock_adder_pc_plus_4();
    datapath.tock_adder_pc_plus_immediate();
    datapath.tock_data_mem_out();

    dmem.read_enable  = ctlpath.data_mem_read_enable;
    dmem.write_enable = ctlpath.data_mem_write_enable;
    dmem.data_format  = datapath.inst_funct3;
    dmem.address      = datapath.data_mem_address;
    dmem.write_data   = datapath.data_mem_write_data;
    dmem.tock1();

    ctlpath.alu_result_equal_zero = datapath.alu_result_equal_zero;
    ctlpath.tock2();

    //----------

    bus_address      = dmem.bus_address;
    bus_write_data   = dmem.bus_write_data;
    bus_byte_enable  = dmem.bus_byte_enable;
    bus_read_enable  = dmem.bus_read_enable;
    bus_write_enable = dmem.bus_write_enable;
  }

  void tock3() {
    dmem.bus_read_data = bus_read_data;
    dmem.tock2();

    datapath.next_pc_select       = ctlpath.next_pc_select;
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



#if 0

riscv_core.clock            => singlecycle_datapath.clock, data_memory_interface.clock;
riscv_core.reset            => singlecycle_datapath.reset;
riscv_core.bus_address      <= data_memory_interface.bus_address;
riscv_core.bus_read_data    => data_memory_interface.bus_read_data;
riscv_core.bus_write_data   <= data_memory_interface.bus_write_data;
riscv_core.bus_byte_enable  <= data_memory_interface.bus_byte_enable
riscv_core.bus_read_enable  <= data_memory_interface.bus_read_enable;
riscv_core.bus_write_enable <= data_memory_interface.bus_write_enable;
riscv_core.inst             => singlecycle_datapath.inst;
riscv_core.pc               <= singlecycle_datapath.pc;


singlecycle_datapath.clock                 <= riscv_core.clock;
singlecycle_datapath.reset                 <= riscv_core.reset;
singlecycle_datapath.inst                  <= riscv_core.inst;
singlecycle_datapath.data_mem_read_data    <= data_memory_interface.read_data;
singlecycle_datapath.data_mem_address      => data_memory_interface.address;
singlecycle_datapath.data_mem_write_data   => data_memory_interface.write_data;
singlecycle_datapath.pc                    => riscv_core.pc;
singlecycle_datapath.inst_opcode           => singlecycle_ctlpath.inst_opcode;
singlecycle_datapath.inst_funct3           => singlecycle_ctlpath.inst_funct3, data_memory_interface.inst_funct3;
singlecycle_datapath.inst_funct7           => singlecycle_ctlpath.inst_funct7;
singlecycle_datapath.pc_write_enable       <= singlecycle_ctlpath.pc_write_enable;
singlecycle_datapath.regfile_write_enable  <= singlecycle_ctlpath.regfile_write_enable;
singlecycle_datapath.alu_operand_a_select  <= singlecycle_ctlpath.alu_operand_a_select;
singlecycle_datapath.alu_operand_b_select  <= singlecycle_ctlpath.alu_operand_b_select;
singlecycle_datapath.reg_writeback_select  <= singlecycle_ctlpath.reg_writeback_select;
singlecycle_datapath.next_pc_select        <= singlecycle_ctlpath.next_pc_select;
singlecycle_datapath.alu-result_equal_zero => singlecycle_ctlpath.alu_result_equal_zero;
singlecycle_datapath.alu_function          <= singlecycle_ctlpath.alu_function;


singlecycle_ctlpath.inst_opcode           <= singlecycle_datapath.inst_opcode;
singlecycle_ctlpath.inst_funct3           <= singlecycle_datapath.inst_funct3;
singlecycle_ctlpath.inst_funct7           <= singlecycle_datapath.inst_funct7;
singlecycle_ctlpath.alu_result_equal_zero <= singlecycle_datapath.alu_result_equal_zero;
singlecycle_ctlpath.pc_write_enable       => singlecycle_datapath.pc_write_enable;
singlecycle_ctlpath.regfile_write_enable  => singlecycle_datapath.regfile_write_enable;
singlecycle_ctlpath.alu_operand_a_select  => singlecycle_datapath.alu_operand_a_select;
singlecycle_ctlpath.alu_operand_b_select  => singlecycle_datapath.alu_operand_b_select;
singlecycle_ctlpath.data_mem_read_enable  => data_memory_interface.read_enable;
singlecycle_ctlpath.data_mem_write_enable => data_memory_interface.write_enable;
singlecycle_ctlpath.reg_writeback_select  => singlecycle_datapath.reg_writeback_select;
singlecycle_ctlpath.alu_function          => singlecycle_datapath.alu_function;
singlecycle_ctlpath.next_pc_select        => singlecycle_datapath.next_pc_select;


data_memory_interface.clock               <= riscv_core.clock;
data_memory_interface.read_enable         <= singlecycle_ctlpath.data_mem_read_enable;
data_memory_interface.write_enable        <= singlecycle_ctlpath.data_mem_write_enable;
data_memory_interface.data_format         <= singlecycle_datapath.inst_funct3;
data_memory_interface.address             <= singlecycle_datapath.data_mem_address;
data_memory_interface.write_data          <= singlecycle_datapath.data_mem_write_data;
data_memory_interface.read_data           => singlecycle_datapath.read_data;
data_memory_interface.bus_address         => riscv_core.bus_address;
data_memory_interface.bus_read_data       <= riscv_core.bus_read_data;
data_memory_interface.bus_write_data      => riscv_core.bus_write_data;
data_memory_interface.bus_read_enable     => riscv_core.bus_read_enable;
data_memory_interface.bus_write_enable    => riscv_core.bus_write_enable;
data_memory_interface.bus_byte_enable     => riscv_core.bus_byte_enable;



#endif