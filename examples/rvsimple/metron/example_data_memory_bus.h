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
  logic<32> address;
  logic<1> read_enable;
  logic<1> write_enable;
  logic<4> byte_enable;
  logic<32> write_data;
  logic<32> q;

  void tock_q() {
    data_memory.address = bx<DATA_BITS - 2>(address, 2);
    data_memory.tock_q();
    logic<1> is_data_memory = address >= DATA_BEGIN && DATA_END >= address;
    q = read_enable && is_data_memory ? data_memory.q : b32(DONTCARE);
  }

  void tock() {
    data_memory.wren =
        b1(write_enable && address >= DATA_BEGIN && DATA_END >= address);
    data_memory.byteena = byte_enable;
    data_memory.data = write_data;

    data_memory.tock();
  }

 private:
  example_data_memory data_memory;
};

#endif  // RVSIMPLE_EXAMPLE_DATA_MEMORY_BUS_H
