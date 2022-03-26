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
  logic<32> bus_address;

  logic<32> bus_write_data2()   const { return dmem.bus_write_data; }
  logic<4>  bus_byte_enable2()  const { return dmem.bus_byte_enable; }
  logic<1>  bus_read_enable2()  const { return ctlpath.data_mem_read_enable(); }
  logic<1>  bus_write_enable2() const { return ctlpath.data_mem_write_enable(); }
  logic<32> bus_address2()      const { return datapath.data_mem_address; }
  logic<32> pc()                const { return datapath.pc2(); }

  //----------------------------------------

  void init() {
    datapath.init();
  }

  void tock_submods(logic<1> reset, logic<32> inst) {
    datapath.tock_submods(
      reset,
      inst,
      ctlpath.pc_write_enable2(),
      ctlpath.regfile_write_enable2()
    );
  }

  void tock_execute(logic<32> inst) {
    datapath.tock_decode(inst);
    datapath.tock_regfile(inst);

    ctlpath.tock_decode(
      datapath.inst_opcode2(inst)
    );

    ctlpath.tock_alu_control(
      datapath.inst_funct32(inst),
      datapath.inst_funct72(inst)
    );

    datapath.tock_alu(
      ctlpath.alu_function(),
      ctlpath.alu_operand_a_select(),
      ctlpath.alu_operand_b_select()
    );

    ctlpath.tock_next_pc_select(
      datapath.inst_opcode2(inst),
      datapath.inst_funct32(inst),
      datapath.alu_result_equal_zero2()
    );

    datapath.tock_next_pc(ctlpath.next_pc_select());

    dmem.tock_bus(
      datapath.inst_funct32(inst),
      datapath.data_mem_address,
      datapath.data_mem_write_data2());

    bus_address = datapath.data_mem_address;
  }

  void tock_writeback(logic<32> inst, logic<32> bus_read_data) {
    dmem.tock_position_fix(datapath.data_mem_address, bus_read_data);
    dmem.tock_sign_fix(datapath.inst_funct32(inst));
    dmem.tock_read_data();
    datapath.tock_writeback(dmem.read_data, ctlpath.reg_writeback_select());
  }

  //----------------------------------------

 private:
  singlecycle_datapath datapath;
  singlecycle_ctlpath ctlpath;
  data_memory_interface dmem;
};

#endif  // RVSIMPLE_RISCV_CORE_H
