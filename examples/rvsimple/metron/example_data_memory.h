// RISC-V SiMPLE SV -- data memory model
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

#ifndef EXAMPLE_DATA_MEMORY_H
#define EXAMPLE_DATA_MEMORY_H

#include "config.h"
#include "constants.h"
#include "metron/metron_tools.h"

class example_data_memory {
 public:
  logic<rv_config::DATA_BITS - 2> address;
  logic<32> q;
  logic<1> wren;
  logic<4> byteena;
  logic<32> data;

 private:
  /*#(* nomem2reg *)#*/
  logic<32> mem_[pow2(rv_config::DATA_BITS - 2)];

 public:
  void tock() {
    q = mem_[address];
    tick();
  }

 private:
  void tick() {
    if (wren) {
      // doing this slightly differently from rvsimple so we don't have to do
      // sub-array writes to mem_.
      logic<32> mask = 0;
      if (byteena[0]) mask = mask | 0x000000FF;
      if (byteena[1]) mask = mask | 0x0000FF00;
      if (byteena[2]) mask = mask | 0x00FF0000;
      if (byteena[3]) mask = mask | 0xFF000000;
      mem_[address] = (mem_[address] & ~mask) | (data & mask);
    }
  }

 public:
  example_data_memory(const char* filename = nullptr) {
    if (filename) readmemh(filename, mem_);
  }
};

#endif // EXAMPLE_DATA_MEMORY_H
