// RISC-V SiMPLE SV -- text memory model
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

#ifndef EXAMPLE_TEXT_MEMORY_H
#define EXAMPLE_TEXT_MEMORY_H

#include "config.h"
#include "constants.h"
#include "metron/tools/metron_tools.h"

class example_text_memory {
 public:
  example_text_memory(const char* filename = nullptr) {
    if (filename) readmemh(filename, mem);
  }

  logic<rv_config::TEXT_BITS - 2> address;
  logic<32> q;
  void tock_q() { q = mem[address]; }

 private:
  logic<32> mem[pow2(rv_config::TEXT_BITS - 2)];
};

#endif // EXAMPLE_TEXT_MEMORY_H
