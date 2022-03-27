// RISC-V SiMPLE SV -- data memory bus
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

#ifndef RVSIMPLE_EXAMPLE_DATA_MEMORY_BUS_H
#define RVSIMPLE_EXAMPLE_DATA_MEMORY_BUS_H

#include "config.h"
#include "constants.h"
#include "example_data_memory.h"
#include "metron_tools.h"

class example_data_memory_bus {
 public:
  logic<32> read_data;

  void init() { data_memory.init(); }

  void tock_data_memory(logic<32> address, logic<1> write_enable, logic<4> byte_enable,
            logic<32> write_data) {
    data_memory.tick(
        bx<DATA_BITS - 2>(address, 2),
        b1(write_enable && address >= DATA_BEGIN && DATA_END >= address),
        byte_enable, write_data);
  }

  example_data_memory data_memory;

  void tock(logic<32> address, logic<1> read_enable) {
    logic<32> fetched = data_memory.q(bx<DATA_BITS - 2>(address, 2));
    logic<1> is_data_memory = address >= DATA_BEGIN && DATA_END >= address;
    read_data =
      read_enable && is_data_memory
        ? fetched
        : b32(DONTCARE);
  }

};

#endif  // RVSIMPLE_EXAMPLE_DATA_MEMORY_BUS_H
