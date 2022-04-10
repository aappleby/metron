// RISC-V SiMPLE SV -- program text memory bus
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

#ifndef RVSIMPLE_EXAMPLE_TEXT_MEMORY_BUS_H
#define RVSIMPLE_EXAMPLE_TEXT_MEMORY_BUS_H

#include "config.h"
#include "constants.h"
#include "example_text_memory.h"
#include "metron_tools.h"

class example_text_memory_bus {
 public:

   logic<32> address;
   logic<32> read_data;
  void tock_read_data() {
    logic<32> fetched = text_memory.q(bx<TEXT_BITS - 2>(address, 2));
    logic<1> is_text_addr = (address >= TEXT_BEGIN) && (TEXT_END >= address);
    read_data = is_text_addr ? fetched : b32(DONTCARE);
  }

 private:
  example_text_memory text_memory;
};

#endif  // RVSIMPLE_EXAMPLE_TEXT_MEMORY_BUS_H
