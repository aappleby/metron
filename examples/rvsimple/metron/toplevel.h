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
  logic<1>  reset;
  logic<32> bus_read_data;
  logic<32> bus_address;
  logic<32> bus_write_data;
  logic<4>  bus_byte_enable;
  logic<1>  bus_read_enable;
  logic<1>  bus_write_enable;

  logic<32> inst;
  logic<32> pc;

  //----------------------------------------

  void tock() {
    core.reset = reset;
    core.tock_pc();

    text_memory_bus.address = core.pc;
    text_memory_bus.tock_read_data();

    core.inst = text_memory_bus.read_data;
    core.tock_execute();

    data_memory_bus.address      = core.bus_address;
    data_memory_bus.read_enable  = core.bus_read_enable;
    data_memory_bus.write_enable = core.bus_write_enable;
    data_memory_bus.byte_enable  = core.bus_byte_enable;
    data_memory_bus.write_data   = core.bus_write_data;
    data_memory_bus.tock();

    core.bus_read_data = data_memory_bus.read_data;
    core.tock_writeback();

    //----------

    bus_read_data    = data_memory_bus.read_data;
    bus_address      = core.bus_address;
    bus_write_data   = core.bus_write_data;
    bus_byte_enable  = core.bus_byte_enable;
    bus_read_enable  = core.bus_read_enable;
    bus_write_enable = core.bus_write_enable;
    inst = text_memory_bus.read_data;
    pc   = core.pc;
  }

  //----------------------------------------

 private:
  riscv_core core;
  example_text_memory_bus text_memory_bus;
  example_data_memory_bus data_memory_bus;
};

#endif  // RVSIMPLE_TOPLEVEL_H
