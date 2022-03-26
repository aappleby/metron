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
  logic<32> bus_read_data()    const { return data_memory_bus.read_data; }
  logic<32> bus_address()      const { return core.bus_address2(); }
  logic<32> bus_write_data()   const { return core.bus_write_data2(); }
  logic<4>  bus_byte_enable()  const { return core.bus_byte_enable2(); }
  logic<1>  bus_read_enable()  const { return core.bus_read_enable2(); }
  logic<1>  bus_write_enable() const { return core.bus_write_enable2(); }

  logic<32> inst() const { return text_memory_bus.read_data; }
  logic<32> pc()   const { return core.pc(); }

  //----------------------------------------

  void init() {
    core.init();
    text_memory_bus.init();
    data_memory_bus.init();
  }

  //----------------------------------------

  void tock(logic<1> reset) {
    data_memory_bus.tock_data_memory(
      core.bus_address,
      core.bus_write_enable2(),
      core.bus_byte_enable2(),
      core.bus_write_data2()
    );
    core.tock_pc(reset);
    core.tock_regs(text_memory_bus.read_data);
    text_memory_bus.tock(core.pc());
    core.tock_execute(text_memory_bus.read_data);
    data_memory_bus.tock(core.bus_address, core.bus_read_enable2());
    core.tock_writeback(text_memory_bus.read_data, data_memory_bus.read_data);
  }

  //----------------------------------------

 private:
  riscv_core core;
  example_text_memory_bus text_memory_bus;
  example_data_memory_bus data_memory_bus;
};

#endif  // RVSIMPLE_TOPLEVEL_H
