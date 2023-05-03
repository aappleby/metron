// RISC-V SiMPLE SV -- program text memory bus
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

#ifndef EXAMPLE_TEXT_MEMORY_BUS_H
#define EXAMPLE_TEXT_MEMORY_BUS_H

#include "config.h"
#include "constants.h"
#include "example_text_memory.h"
#include "metron/tools/metron_tools.h"

class example_text_memory_bus {
 public:
  logic<32> address;
  logic<32> read_data;

  example_text_memory_bus(const char* filename = nullptr)
  : text_memory(filename) {
  }

 private:
  example_text_memory text_memory;

 public:
  void tock_read_data() {
    text_memory.address = bx<rv_config::TEXT_BITS - 2>(address, 2);
    text_memory.tock_q();
    logic<32> fetched = text_memory.q;
    read_data =
        (address >= rv_config::TEXT_BEGIN) && (rv_config::TEXT_END >= address)
            ? fetched
            : b32(DONTCARE);
  }
};

#endif // EXAMPLE_TEXT_MEMORY_BUS_H
