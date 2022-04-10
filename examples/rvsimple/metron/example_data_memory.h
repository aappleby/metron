// RISC-V SiMPLE SV -- data memory model
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

#ifndef RVSIMPLE_EXAMPLE_DATA_MEMORY_H
#define RVSIMPLE_EXAMPLE_DATA_MEMORY_H

#include "config.h"
#include "constants.h"
#include "metron_tools.h"

class example_data_memory {
 public:
  example_data_memory() {
    std::string s;
    value_plusargs("data_file=%s", s);
    readmemh(s, mem);
  }

  logic<rv_config::DATA_BITS - 2> address;
  logic<32> q;
  logic<1> wren;
  logic<4> byteena;
  logic<32> data;

  void tock() {
    q = mem[address];
    tick();
  }

 private:
  void tick() {
    if (wren) {
      logic<32> mask = 0;
      if (byteena[0]) mask = mask | 0x000000FF;
      if (byteena[1]) mask = mask | 0x0000FF00;
      if (byteena[2]) mask = mask | 0x00FF0000;
      if (byteena[3]) mask = mask | 0xFF000000;
      mem[address] = (mem[address] & ~mask) | (data & mask);
    }
  }

  logic<32> mem[pow2(rv_config::DATA_BITS - 2)];
};

#endif  // RVSIMPLE_EXAMPLE_DATA_MEMORY_H
