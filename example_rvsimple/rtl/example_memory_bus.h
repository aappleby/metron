// RISC-V SiMPLE SV -- combined text/data memory bus
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

#ifndef RVSIMPLE_EXAMPLE_MEMORY_BUS_H
#define RVSIMPLE_EXAMPLE_MEMORY_BUS_H

#include "config.h"
#include "constants.h"
#include "example_data_memory.h"
#include "example_text_memory.h"
#include "metron_tools.h"

class example_memory_bus {
 public:
  logic<32> read_data;

  example_data_memory data_memory;
  example_text_memory text_memory;

  void tick(logic<32> address, logic<1> write_enable, logic<4> byte_enable,
            logic<32> write_data) {
    data_memory.tick(
        bx<DATA_BITS - 2>(address, 2),
        write_enable && address >= DATA_BEGIN && address <= DATA_END,
        byte_enable, write_data);
  }

  void tock(logic<32> address, logic<1> read_enable) {
    data_memory.tock(bx<DATA_BITS - 2>(address, 2));
    text_memory.tock(bx<TEXT_BITS - 2>(address, 2));

    logic<32> text_fetched = text_memory.q;
    logic<32> data_fetched = data_memory.q;

    logic<1> is_data_memory = address >= DATA_BEGIN && address <= DATA_END;
    logic<1> is_text_memory = address >= TEXT_BEGIN && address <= TEXT_END;

    if (read_enable) {
      if (is_text_memory)
        read_data = text_memory.q;
      else if (is_data_memory)
        read_data = data_memory.q;
      else
        read_data = b32(DONTCARE);
    } else {
      read_data = b32(DONTCARE);
    }
  }
};

#endif  // RVSIMPLE_EXAMPLE_MEMORY_BUS_H