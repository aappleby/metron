// RISC-V SiMPLE SV -- Toplevel
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

#ifndef RVSIMPLE_TOPLEVEL_H
#define RVSIMPLE_TOPLEVEL_H

#include "config.h"
#include "constants.h"
#include "example_data_memory_bus.h"
#include "example_text_memory_bus.h"
#include "metron_tools.h"
#include "riscv_core.h"

class toplevel {
 public:
  logic<32> bus_read_data;
  logic<32> bus_address;
  logic<32> bus_write_data;
  logic<4>  bus_byte_enable;
  logic<1>  bus_read_enable;
  logic<1>  bus_write_enable;
  logic<32> inst;
  logic<32> pc;

  //----------------------------------------

  void init() {
    core.init();
    text_memory_bus.init();
    data_memory_bus.init();
  }

  //----------------------------------------

  void tock(logic<1> reset) {
    text_memory_bus.tock(core.pc());
    inst = text_memory_bus.read_data;

    core.tock_execute(inst);
    core.tock_writeback(inst, data_memory_bus.read_data(core.bus_address, core.bus_read_enable2()));

    bus_read_data = data_memory_bus.read_data(core.bus_address, core.bus_read_enable2());
    bus_address = core.bus_address2();
    bus_write_data = core.bus_write_data2(inst);
    bus_byte_enable = core.bus_byte_enable2(inst);
    bus_read_enable = core.bus_read_enable2();
    bus_write_enable = core.bus_write_enable2();
    pc = core.pc();
    core.tock_pc(reset);
    data_memory_bus.tock_data_memory(
      core.bus_address,
      core.bus_write_enable2(),
      core.bus_byte_enable2(inst),
      core.bus_write_data2(inst)
    );
    core.tock_regs(inst);
  }

  //----------------------------------------

 private:
  riscv_core core;
  example_text_memory_bus text_memory_bus;
  example_data_memory_bus data_memory_bus;
};

#endif  // RVSIMPLE_TOPLEVEL_H
